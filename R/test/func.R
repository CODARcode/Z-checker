# This function is invoked by the C program r_test

add1 <- function(a, b=1) {
  cat("a received: ", a, "\n");
  cat("b received: ", b, "\n");

  return(a + b)
}

computeErr <- function(a,b=0){
	return (a-b);
}


