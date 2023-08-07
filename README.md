# Gora off-chain computation development starter

This repository is intended as the starting point for anyone willing to use Gora
off-chain computation in their smart contracts. Here you will find:

 * Overview of the Gora off-chain computation feature
 * Technical details required to use it
 * Example off-chain programs written in C language

## Overview

Off-chain computation is a feature of the Gora network that allows executing
user-supplied code as a part of an oracle request. Normal oracle requests can
only apply basic aggregation functions to values they receive from data sources.

Off-chain computation requests can process received values in arbitrary ways, or
even decide what data sources they query with what parameters at run
time. Developers compile their off-chain code to a [Web Assembly](https://webassembly.org/)
binary (a `.wasm` file) and supply it with their oracle request in one of the
three ways: directly in a smart contract call parameter, in an Algorand *storage
box* they reference, or simply as an URL from where it can be retrieved. Any
language can be used for writing off-chain code as long as it compiles to web
assembly, but our examples use C with the [Clang](https://clang.llvm.org/)
compiler because it does not require additional tooling and strikes a good
balance between language features and executable size. To help fitting
executables in the limited Algorand smart contract parameter space or saving on
box storage costs, they can be compressed with `gzip` to be automatically
detected and decompressed before running.

## Specifying an off-chain computation request

Like other types of Gora oracle requests, off-chain requests are made by calling
the `request` method of main Gora smart contract. The `request_type` call
parameter must be set to `3`, signifying that it is an off-chain computation
request. The `request_args` argument is populated with request specification
which is the encoded instance of the following *Algorand ABI* type:

 * `apiVersion: uint32` - minimum off-chain API version required, currently must
   be set to `1`.
 * `execSpecType: uint8` - format of executable specification type contained in
 * `execSpec` field below (`0` - in-place executable (web assembly binary itself);
   `1` - Algorand storage box specification  (8-byte box owner app ID, followed
    by box name); `2` - URL)
 * `execSpec: byte[]` - executable specification
 * `execArgs: dynamic array of byte[]` - positional arguments to the off-chain code
 * `userData: byte[]` - user-supplied data, passed to destination app as is
 * `reserved0: uint32` - reserved for future use
 * `reserved1: uint32` - reserved for future use
 * `reserved2: byte[]` - reserved for future use
 * `reserved3: byte[]` - reserved for future use


Apart from the above details, making an off-chain computation request no
different than for other request types.

## Usage examples

This repository contains two example off-chain computation programs:
`basic_example.c` and `multi_step_example.c`.  The first example just outputs
the phrase "Hello world!" as an oracle value and is self-explanatory. The second
one actually does useful work, so it is extensively commented and will be
considered here in detail.

The `multi_step_example.c` off-chain program takes a British postcode as a
parameter, queries two data sources, building their URLs dynamically, and
returns current air temperature in are of the specified postcode area.

This must be done in two stages: getting postcode geographical coordinates and
querying current weather at them. Because of certain inherent limitations of Web
Assembly, programs cannot efficiently pause while waiting to receive data from
extrnal sources such as URLs. To work around that, Gora off-chain programs are
run in *steps*. Steps are essentially repeated executions of the program with a
shared context that includes current execution number.

During a step, the program can schedule HTTP(S) requests using URLs templates
that are filled at run time. When the step ends, these requests are executed by
the Gora node. The program can access results of HTTP requests scheduled at the
previous step as well as other node-provided data such as the number of step
currently executing via the structure passed to it as an argument. Finishing a
step, the program always returns a value which tells the Gora node what to do
next: execute another step, finish successfully or terminate with a specified
error code. For list of available error codes, consider `gora_off_chain.h`
header file.

To compile the example, run Clang C compiler version 12 or newer:
```
clang multi_step_example.c -Os --target=wasm32-unknown-unknown-wasm -c -o multi_step_example.wasm
```
To execute the compiled binary using default test destination app, run:
```
node cli request --off-chain ./off_chain_example.wasm --arg sm14hp
```

To generate test requests from `JavaScript` the followin snippet can be used:
```
  const wasmCode = Fs.readFileSync("multi_step_example.wasm");
  const req = new Goracle.OracleRequest({
    type: 3,
    destAppId: 161963900, // dummy destination app ID
    destMeth: "exampleMethod",
    execSpecType: 0,
    execSpec: wasmCode,
    execArgs: [ "sm14hp" ]
  });
```
