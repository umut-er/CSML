# CSML

CSML (Client Server Model w/ Lobbies) is a client server model framework written in C / works natively with C++. Lobbies have been implemented to compartmentalize clients. This insures that they can be served according to the secret state of their lobby. 

## How Does It Work?
A CSML_Server is a conglomerate of different CSML_Lobby objects. Each lobby owns a secret state, which the user can initialize. On top of this, each lobby has an array of RPtr (Response Pointer) objects, which are just function pointers.Each function responds to exactly one request code [see Definitions and Clarifications below]. These functions take the server's secret state and the request as input. These functions are allowed to modify the secret state and send requests to clients.  
This way, each lobby can be arbitrarily customized. A RPtr must be added individually to a lobby in order to serve that lobby's clients. One may create completely identical lobby objects that respond identically or completely different lobby objects, according to what the user needs.

## Who Should Use This Project?
This project is currently in it's very early stages and it's completely experimental. However, if you wish, you can use it. You are not guaranteed a bugless experience, however.  
If you want to contribute, open an issue or an pull request. I will probably see it.

### Definitions and Clarifications
- A CSML response should be structured as follows (this might change as the project develops):  
"CODE MESSAGE"  
CODE (also referred to as the "request code") is an unique integer, denoting the type of the request.  
MESSAGE is the actual content of the request.
- `xbase_server.c` and `xexample_client.c` files are not a part of this project. They are just demonstrations of the project.
- This project is released under an "Unlicense License". That means this project is completely public domain. Do whatever you want with it.
