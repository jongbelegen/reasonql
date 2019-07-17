let gql = code => code;

type apolloErrorJs = {
  .
  "message": string,
  "extensions": Js.Json.t,
};

type apolloError = {
  message: string,
  extensions: Js.Json.t,
};

let decodeError: array(apolloErrorJs) => array(apolloError) =
  errors => {
    errors
    |> Array.map(err => {message: err##message, extensions: err##extensions});
  };

module type Client = {
  let url: string;
  let headers: Js.t({..});
};

module type Query = {
  let query: string;

  type variablesType;
  let encodeVariables: variablesType => Js.Json.t;

  type queryResult;
  let decodeQueryResult: Js.Json.t => queryResult;
};

module MakeRequest = (Q: Query, C: Client) => {
  type apolloResultJs = {
    .
    "data": Js.Json.t,
    "errors": option(array(apolloErrorJs)),
  };

  type response = {
    .
    [@bs.meth] "json": unit => Js.Promise.t(apolloResultJs),
  };

  [@bs.val]
  external fetch: (string, Js.Json.t) => Js.Promise.t(response) = "";

  let send =
      (~headers=Js.Obj.empty(), ~vars: Q.variablesType)
      : Js.Promise.t(apolloResultJs) => {
    Js.Promise.(
      fetch(
        C.url,
        Obj.magic({
          "method": "POST",
          "headers":
            Js.Obj.assign(
              Js.Obj.assign({"Content-Type": "application/json"}, C.headers),
              headers,
            ),
          "body":
            Js.Json.stringify(
              Obj.magic({
                "query": Q.query,
                "variables": Q.encodeVariables(vars),
              }),
            ),
        }),
      )
      |> then_((response: response) => response##json())
    );
  };

  let finishedPromise:
    Js.Promise.t(apolloResultJs) => Js.Promise.t(Q.queryResult) =
    promise => {
      Js.Promise.(
        promise
        |> then_(json => {
             let data = Q.decodeQueryResult(json##data);
             resolve(data);
           })
      );
    };

  let finished: (Js.Promise.t(apolloResultJs), Q.queryResult => unit) => unit =
    (result, callback) =>
      Js.Promise.(
        result
        |> finishedPromise
        |> then_(decoded => callback(decoded) |> resolve)
        |> ignore
      );

  let finishedWithErrorPromise:
    Js.Promise.t(apolloResultJs) =>
    Js.Promise.t((option(Q.queryResult), option(array(apolloError)))) =
    promise => {
      Js.Promise.(
        promise
        |> then_(json => {
             let errors =
               switch (json##errors) {
               | Some(errors) => Some(decodeError(errors))
               | None => None
               };
             let data =
               errors == None ? Some(Q.decodeQueryResult(json##data)) : None;
             resolve((data, errors));
           })
      );
    };

  let finishedWithError:
    (
      Js.Promise.t(apolloResultJs),
      (option(Q.queryResult), option(array(apolloError))) => unit
    ) =>
    unit =
    (result, callback) =>
      Js.Promise.(
        result
        |> finishedWithErrorPromise
        |> then_(((decoded, error)) => callback(decoded, error) |> resolve)
        |> ignore
      );
};