
basicStats = function(a,b){  # a and b are of any dimension 
print('-- Basic Statisitics --')
print('Range of values of a')
print(range(a,na.rm=TRUE))
print('Range of values of b')
print(range(b,na.rm=TRUE))
print('Mean of samples a and b')
print(c(mean(c(a),na.rm=TRUE),mean(c(b),na.rm=TRUE)))
print('Variance of samples a and b')
print(c(var(c(a),na.rm=TRUE),var(c(b),na.rm=TRUE)))
print('Correlation between samples a and b')
print(cor(c(a),c(b)))
}



KS_test = function(a,b){  # a and b are of any dimension 
    KS = ks.test(x=c(a),y=c(b))
    pval = KS$p.value
    #print('Kolmogorov-Smirnov test p-value')
    #print(pval)
    pval
}


SSIM = function(a,b,alpha=1,beta=1,gamma=1){  # a and b are 1D-array representing a multi-D image/quantity
L = max(c(a,b))
k1 = 0.01
k2 = 0.03
c1 = (k1*L)^2
c2 = (k2*L)^2
c3 = c2/2
mu_a = mean(c(a),na.rm=TRUE)
mu_b = mean(c(b),na.rm=TRUE)
s_a = sd(c(a),na.rm=TRUE)
s_b = sd(c(b),na.rm=TRUE)
s_ab = cov(c(a),c(b))
lum = (2*mu_a*mu_b + c1)/(mu_a^2 + mu_b^2 + c1)
#print('Luminance')
#print(lum)
cont = (2*s_a*s_b + c2)/(s_a^2 + s_b^2 + c2)
#print('Contrast')
#print(cont)
struc = (s_ab + c3)/(s_a*s_b + c3)
#print('Structure')
#print(struc)
ssim = (lum^alpha) * (cont^beta) * (struc^gamma)
#print('SSIM')
#print(ssim)
c(lum,cont,struc,ssim)  }

