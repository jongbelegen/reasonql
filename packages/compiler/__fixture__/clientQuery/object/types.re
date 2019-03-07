/* Generated by Reason Relay Compiler, PLEASE EDIT WITH CARE */

let query = {|
query types($j: Int!, $ss: String) {
  i
  a(j:$j) {
    id
    ii
    b {
      id
      iii
    }
    ff
  }
  s
  c(ss:$ss) {
    id
    ss
  }
}
|}

type a_b_B = {
  id: string,
  iii: option(int),
};

type a_A = {
  id: string,
  ii: option(int),
  b: a_b_B,
  ff: float,
};

type c_C = {
  id: string,
  ss: option(string),
};

type queryResponse = {
  i: int,
  a: option(a_A),
  s: option(string),
  c: c_C,
};

type variablesType = {
  j: int,
  ss: option(string),
};

[@bs.deriving abstract]
type queryVars = {
  j: int,
  ss: option(string),
};

let encodeVariables: variablesType => queryVars = (vars) => variablesType(~j=vars.j,~ss=vars.ss);

type schemaQueryResponse = SchemaTypes.queryResponse;
let decodeResponse = SchemaTypes.decodeQueryResponse;