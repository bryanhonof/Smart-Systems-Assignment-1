# CMD to POST
```sh
curl --header "Content-Type: application/json" \
     --request POST \
     --data '{"value":"42","place":"Maasmechelen"}'
     https://<YOURLINK>.herokuapp.com/api/temp
```

#CMD to PUT
```sh
curl --header "Content-Type: application/json" \
     --request PUT \
     --data '{"value":"42","place":"Maasmechelen"}'
     https://<YOURLINK>.herokuapp.com/api/temp/<ID>
```

#CMD to PUT
```sh
curl --header "Content-Type: application/json" \
     --request DELETE \
     https://<YOURLINK>.herokuapp.com/api/temp/<ID>
```
