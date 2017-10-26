# This function is invoked by the C program r_test

add1 <- function(a, b=1,c=1,d=1,e=1,f=1) {
  cat("a received: ", a, "\n");
  cat("b received: ", b, "\n");
  cat("c received: ", c, "\n");
  cat("d received: ", d, "\n");
  cat("e received: ", e, "\n");
  cat("f received: ", f, "\n");
  return(a + b + c + d + e + f)
}

computeErr <- function(a,b=0){
	return (a-b);
}


