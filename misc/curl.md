# curl HTTP commands

## CMD to POST

```zsh
curl --header "Content-Type: application/json" \
     --request POST \
     --data '{"value":"42","place":"Maasmechelen"}'
     https://<YOURLINK>.herokuapp.com/api/temp
```

## CMD to PUT

```zsh
curl --header "Content-Type: application/json" \
     --request PUT \
     --data '{"value":"42","place":"Maasmechelen"}'
     https://<YOURLINK>.herokuapp.com/api/temp/<ID>
```

## CMD to DELETE 

```zsh
curl --header "Content-Type: application/json" \
     --request DELETE \
     https://<YOURLINK>.herokuapp.com/api/temp/<ID>
```
