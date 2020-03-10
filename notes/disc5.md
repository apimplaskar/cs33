# Discussion Week 5
---
### Loop unrolling/Reassociated computation
- Basic example of finding product of all elements of a list
-
```{c}
int prod = 1;
for(i = 0; i < n; i++){
  prod = prod * arr[i];
}
```
-
```{c}
int prod = 1;
for(i = 0; i < n; i+=2){
  prod = prod *(arr[i] * arr[i+1]); //two elements at once
}
```
-
```{c}
for(i = 0; i < limit; i += 2){
  x0 = x0 * arr[i];
  x1 = x1 * arr[i+1];
}
prod = x0 * x1;
```
- Find ways to exploit use of multiple threads to make program more efficient
