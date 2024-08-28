# Wordle in Your Terminal!

## Play locally or using an API to connect to New York Times' Daily Wordle Answer

### Dependencies
- curl
- jq
- clang

### To Use
1. Dowload all files
2. navigate to the directory with the file and run `make` to create the local execuatable
3. run `make api` to make the api-wordle executable
4. run either `./wordle` or `./api-wordle` to play the games!

## Notes
- Yes it actually retrieves the NYT daily wordle
- I found the easiest way to retrieve data from an api in C was to run bash command, specifically curl and jq
