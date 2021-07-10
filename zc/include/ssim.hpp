#ifndef SSIM_HPP
#define SSIM_HPP


template <typename dT>
double matrix<dT>::SSIM_4d_windowed(matrix &other, int windowSize0, int windowSize1, int windowSize2, int windowSize3, int windowShift0, int windowShift1, int windowShift2, int windowShift3) {
    int offset0,offset1,offset2,offset3;
    int nw=0; //Number of windows
    double ssimSum=0;
    int offsetInc0,offsetInc1,offsetInc2,offsetInc3;

    if(windowSize0>size0) {
        cout<<"ERROR: windowSize0 = "<<windowSize0<<" > "<<size0<<" = matrix::size0"<<endl;
        assert(0);
    }
    if(windowSize1>size1) {
        cout<<"ERROR: windowSize1 = "<<windowSize1<<" > "<<size1<<" = matrix::size1"<<endl;
        assert(0);
    }
    if(windowSize2>size2) {
        cout<<"ERROR: windowSize2 = "<<windowSize2<<" > "<<size2<<" = matrix::size2"<<endl;
        assert(0);
    }
    if(windowSize3>size3) {
        cout<<"ERROR: windowSize3 = "<<windowSize3<<" > "<<size3<<" = matrix::size3"<<endl;
        assert(0);
    }

    //offsetInc0=windowSize0/2;
    //offsetInc1=windowSize1/2;
    //offsetInc2=windowSize2/2;
    //offsetInc3=windowSize3/2;
    offsetInc0=windowShift0;
    offsetInc1=windowShift1;
    offsetInc2=windowShift2;
    offsetInc3=windowShift3;

    for(offset3=0; offset3+windowSize3<=size3; offset3+=offsetInc3) { //MOVING WINDOW

        for(offset2=0; offset2+windowSize2<=size2; offset2+=offsetInc2) { //MOVING WINDOW

            for(offset1=0; offset1+windowSize1<=size1; offset1+=offsetInc1) { //MOVING WINDOW

                for(offset0=0; offset0+windowSize0<=size0; offset0+=offsetInc0) { //MOVING WINDOW
                    nw++;
                    ssimSum+=SSIM_4d_calcWindow(other, offset0, offset1, offset2, offset3, windowSize0, windowSize1, windowSize2, windowSize3);
                    //ssimSum+=SSIM_4d_calcWindow<dT>(other,offset0,offset1,offset2,offset3,windowSize0,windowSize1,windowSize2,windowSize3,size0,size1,size2).ssim;
                }
            }
        }
    }

    //cout<<"# of windows = "<<nw<<endl;
    return ssimSum/nw;
    return 0;
}

template <typename dT>
double matrix<dT>::SSIM_4d_calcWindow(matrix &other, int offset0, int offset1, int offset2, int offset3,int windowSize0, int windowSize1, int windowSize2, int windowSize3) {
    int i0,i1,i2,i3,index;
    int np=0; //Number of points
    dT xMin=data[offset0+size0*(offset1+size1*(offset2+size2*offset3))];
    dT xMax=data[offset0+size0*(offset1+size1*(offset2+size2*offset3))];
    dT yMin=data[offset0+size0*(offset1+size1*(offset2+size2*offset3))];
    dT yMax=data[offset0+size0*(offset1+size1*(offset2+size2*offset3))];
    double xSum=0;
    double ySum=0;
    for(i3=offset3; i3<offset3+windowSize3; i3++) {
        for(i2=offset2; i2<offset2+windowSize2; i2++) {
            for(i1=offset1; i1<offset1+windowSize1; i1++) {
                for(i0=offset0; i0<offset0+windowSize0; i0++) {
                    np++;
                    index=i0+size0*(i1+size1*(i2+size2*i3));
                    if(xMin>data[index])
                        xMin=data[index];
                    if(xMax<data[index])
                        xMax=data[index];
                    if(yMin>other.data[index])
                        yMin=other.data[index];
                    if(yMax<other.data[index])
                        yMax=other.data[index];
                    xSum+=data[index];
                    ySum+=other.data[index];
                }
            }
        }
    }

    double xMean=xSum/np;
    double yMean=ySum/np;
    double var_x = 0, var_y = 0, var_xy = 0;

    for(i3=offset3; i3<offset3+windowSize3; i3++) {
        for(i2=offset2; i2<offset2+windowSize2; i2++) {
            for(i1=offset1; i1<offset1+windowSize1; i1++) {
                for(i0=offset0; i0<offset0+windowSize0; i0++) {
                    index=i0+size0*(i1+size1*(i2+size2*i3));
                    var_x += (data[index] - xMean)*(data[index] - xMean);
                    var_y += (other.data[index] - yMean)*(other.data[index] - yMean);
                    var_xy += (data[index] - xMean)*(other.data[index] - yMean);
                }
            }
        }
    }

    var_x /= np;
    var_y /= np;
    var_xy /= np;

    double xSigma=sqrt(var_x);
    double ySigma=sqrt(var_y);
    double xyCov = var_xy;

    double c1,c2;
    if(xMax-xMin==0) {
        c1=K1*K1;
        c2=K2*K2;
    } else {
        c1=K1*K1*(xMax-xMin)*(xMax-xMin);
        c2=K2*K2*(xMax-xMin)*(xMax-xMin);
    }
    double c3=c2/2;

    double luminance=(2*xMean*yMean+c1)/(xMean*xMean+yMean*yMean+c1);
    double contrast=(2*xSigma*ySigma+c2)/(xSigma*xSigma+ySigma*ySigma+c2);
    double structure=(xyCov+c3)/(xSigma*ySigma+c3);
    double ssim=luminance*contrast*structure;
    return ssim;
}


template <typename dT>
double matrix<dT>::SSIM_3d_windowed(matrix &other, int windowSize0, int windowSize1, int windowSize2, int windowShift0, int windowShift1, int windowShift2) {
    int offset0,offset1,offset2;
    int nw=0; //Number of windows
    double ssimSum=0;
    int offsetInc0,offsetInc1,offsetInc2;

    if(windowSize0>size0) {
        cout<<"ERROR: windowSize0 = "<<windowSize0<<" > "<<size0<<" = matrix::size0"<<endl;
        assert(0);
    }
    if(windowSize1>size1) {
        cout<<"ERROR: windowSize1 = "<<windowSize1<<" > "<<size1<<" = matrix::size1"<<endl;
        assert(0);
    }
    if(windowSize2>size2) {
        cout<<"ERROR: windowSize2 = "<<windowSize2<<" > "<<size2<<" = matrix::size2"<<endl;
        assert(0);
    }

    //offsetInc0=windowSize0/2;
    //offsetInc1=windowSize1/2;
    //offsetInc2=windowSize2/2;
    offsetInc0=windowShift0;
    offsetInc1=windowShift1;
    offsetInc2=windowShift2;

    for(offset2=0; offset2+windowSize2<=size2; offset2+=offsetInc2) { //MOVING WINDOW

        for(offset1=0; offset1+windowSize1<=size1; offset1+=offsetInc1) { //MOVING WINDOW

            for(offset0=0; offset0+windowSize0<=size0; offset0+=offsetInc0) { //MOVING WINDOW
                nw++;
                ssimSum+=SSIM_3d_calcWindow(other, offset0, offset1, offset2, windowSize0, windowSize1, windowSize2);

            }
        }
    }

    //cout<<"# of windows = "<<nw<<endl;
    return ssimSum/nw;
    return 0;
}

template <typename dT>
double matrix<dT>::SSIM_3d_calcWindow(matrix &other, int offset0, int offset1, int offset2, int windowSize0, int windowSize1, int windowSize2) {
    int i0,i1,i2,index;
    int np=0; //Number of points
    dT xMin=data[offset0+size0*(offset1+size1*offset2)];
    dT xMax=data[offset0+size0*(offset1+size1*offset2)];
    dT yMin=data[offset0+size0*(offset1+size1*offset2)];
    dT yMax=data[offset0+size0*(offset1+size1*offset2)];
    double xSum=0;
    double ySum=0;

    for(i2=offset2; i2<offset2+windowSize2; i2++) {
        for(i1=offset1; i1<offset1+windowSize1; i1++) {
            for(i0=offset0; i0<offset0+windowSize0; i0++) {
                np++;
                index=i0+size0*(i1+size1*i2);
                if(xMin>data[index])
                    xMin=data[index];
                if(xMax<data[index])
                    xMax=data[index];
                if(yMin>other.data[index])
                    yMin=other.data[index];
                if(yMax<other.data[index])
                    yMax=other.data[index];
                xSum+=data[index];
                ySum+=other.data[index];
            }
        }
    }

    double xMean=xSum/np;
    double yMean=ySum/np;
    double var_x = 0, var_y = 0, var_xy = 0;

    for(i2=offset2; i2<offset2+windowSize2; i2++) {
        for(i1=offset1; i1<offset1+windowSize1; i1++) {
            for(i0=offset0; i0<offset0+windowSize0; i0++) {
                index=i0+size0*(i1+size1*i2);
                var_x += (data[index] - xMean)*(data[index] - xMean);
                var_y += (other.data[index] - yMean)*(other.data[index] - yMean);
                var_xy += (data[index] - xMean)*(other.data[index] - yMean);
            }
        }
    }

    var_x /= np;
    var_y /= np;
    var_xy /= np;

    double xSigma=sqrt(var_x);
    double ySigma=sqrt(var_y);
    double xyCov = var_xy;

    double c1,c2;
    if(xMax-xMin==0) {
        c1=K1*K1;
        c2=K2*K2;
    } else {
        c1=K1*K1*(xMax-xMin)*(xMax-xMin);
        c2=K2*K2*(xMax-xMin)*(xMax-xMin);
    }
    double c3=c2/2;

    double luminance=(2*xMean*yMean+c1)/(xMean*xMean+yMean*yMean+c1);
    double contrast=(2*xSigma*ySigma+c2)/(xSigma*xSigma+ySigma*ySigma+c2);
    double structure=(xyCov+c3)/(xSigma*ySigma+c3);
    double ssim=luminance*contrast*structure;

    //double ssim=luminance*luminance*luminance*luminance;
    //ssim=ssim*contrast*contrast*contrast*contrast;
    //ssim=ssim*structure*structure*structure*structure;
    //cout<<"xMean : yMean : xSigma : ySigma : xyCov = "<<xMean<<" : "<<yMean<<" : "<<xSigma<<" : "<<ySigma<<" : "<<xyCov<<endl;
    //cout<<"l : c : s = "<<luminance<<" : "<<contrast<<" : "<<structure<<endl; //AMG

    //if(isnan(ssim)) assert(0);
    return ssim;
}

template <typename dT>
double matrix<dT>::SSIM_2d_windowed(matrix &other, int windowSize0, int windowSize1, int windowShift0, int windowShift1) {
    int offset0,offset1;
    int nw=0; //Number of windows
    double ssimSum=0;
    int offsetInc0,offsetInc1;

    if(windowSize0>size0) {
        cout<<"ERROR: windowSize0 = "<<windowSize0<<" > "<<size0<<" = matrix::size0"<<endl;
        assert(0);
    }
    if(windowSize1>size1) {
        cout<<"ERROR: windowSize1 = "<<windowSize1<<" > "<<size1<<" = matrix::size1"<<endl;
        assert(0);
    }

    //offsetInc0=windowSize0/2;
    //offsetInc1=windowSize1/2;
    offsetInc0=windowShift0;
    offsetInc1=windowShift1;

    for(offset1=0; offset1+windowSize1<=size1; offset1+=offsetInc1) { //MOVING WINDOW

        for(offset0=0; offset0+windowSize0<=size0; offset0+=offsetInc0) { //MOVING WINDOW
            nw++;
            ssimSum+=SSIM_2d_calcWindow(other, offset0, offset1, windowSize0, windowSize1);
        }
    }

    //cout<<"# of windows = "<<nw<<endl;
    return ssimSum/nw;
    return 0;
}

template <typename dT>
double matrix<dT>::SSIM_2d_calcWindow(matrix &other, int offset0, int offset1, int windowSize0, int windowSize1) {
    int i0,i1,index;
    int np=0; //Number of points
    dT xMin=data[offset0+size0*offset1];
    dT xMax=data[offset0+size0*offset1];
    dT yMin=data[offset0+size0*offset1];
    dT yMax=data[offset0+size0*offset1];
    double xSum=0;
    double ySum=0;

    for(i1=offset1; i1<offset1+windowSize1; i1++) {
        for(i0=offset0; i0<offset0+windowSize0; i0++) {
            np++;
            index=i0+size0*i1;
            if(xMin>data[index])
                xMin=data[index];
            if(xMax<data[index])
                xMax=data[index];
            if(yMin>other.data[index])
                yMin=other.data[index];
            if(yMax<other.data[index])
                yMax=other.data[index];
            xSum+=data[index];
            ySum+=other.data[index];
        }
    }

    double xMean=xSum/np;
    double yMean=ySum/np;

    double var_x = 0, var_y = 0, var_xy = 0;

    for(i1=offset1; i1<offset1+windowSize1; i1++) {
        for(i0=offset0; i0<offset0+windowSize0; i0++) {
            index=i0+size0*i1;
            var_x += (data[index] - xMean)*(data[index] - xMean);
            var_y += (other.data[index] - yMean)*(other.data[index] - yMean);
            var_xy += (data[index] - xMean)*(other.data[index] - yMean);
        }
    }

    var_x /= np;
    var_y /= np;
    var_xy /= np;

    double xSigma=sqrt(var_x);
    double ySigma=sqrt(var_y);
    double xyCov = var_xy;


    double c1,c2;
    if(xMax-xMin==0) {
        c1=K1*K1;
        c2=K2*K2;
    } else {
        c1=K1*K1*(xMax-xMin)*(xMax-xMin);
        c2=K2*K2*(xMax-xMin)*(xMax-xMin);
    }
    double c3=c2/2;

    double luminance=(2*xMean*yMean+c1)/(xMean*xMean+yMean*yMean+c1);
    double contrast=(2*xSigma*ySigma+c2)/(xSigma*xSigma+ySigma*ySigma+c2);
    double structure=(xyCov+c3)/(xSigma*ySigma+c3);
    double ssim=luminance*contrast*structure;
    return ssim;
}

template <typename dT>
double matrix<dT>::SSIM_1d_windowed(matrix &other, int windowSize0, int windowShift0) {
    int offset0;
    int nw=0; //Number of windows
    double ssimSum=0;
    int offsetInc0;

    if(windowSize0>size0) {
        cout<<"ERROR: windowSize0 = "<<windowSize0<<" > "<<size0<<" = matrix::size0"<<endl;
        assert(0);
    }

    //offsetInc0=windowSize0/2;
    offsetInc0=windowShift0;


    for(offset0=0; offset0+windowSize0<=size0; offset0+=offsetInc0) { //MOVING WINDOW
        nw++;
        ssimSum+=SSIM_1d_calcWindow(other, offset0, windowSize0);
    }

    //cout<<"# of windows = "<<nw<<endl;
    return ssimSum/nw;
    return 0;
}

template <typename dT>
double matrix<dT>::SSIM_1d_calcWindow(matrix &other, int offset0, int windowSize0) {
    int i0;
    int np=0; //Number of points
    dT xMin=data[offset0];
    dT xMax=data[offset0];
    dT yMin=data[offset0];
    dT yMax=data[offset0];
    double xSum=0;
    double ySum=0;

    for(i0=offset0; i0<offset0+windowSize0; i0++) {
        np++;
        if(xMin>data[i0])
            xMin=data[i0];
        if(xMax<data[i0])
            xMax=data[i0];
        if(yMin>other.data[i0])
            yMin=other.data[i0];
        if(yMax<other.data[i0])
            yMax=other.data[i0];
        xSum+=data[i0];
        ySum+=other.data[i0];
    }

    double xMean=xSum/np;
    double yMean=ySum/np;

    double var_x = 0, var_y = 0, var_xy = 0;

    for(i0=offset0; i0<offset0+windowSize0; i0++) {
        var_x += (data[i0] - xMean)*(data[i0] - xMean);
        var_y += (other.data[i0] - yMean)*(other.data[i0] - yMean);
        var_xy += (data[i0] - xMean)*(other.data[i0] - yMean);
    }

    var_x /= np;
    var_y /= np;
    var_xy /= np;

    double xSigma=sqrt(var_x);
    double ySigma=sqrt(var_y);
    double xyCov = var_xy;

    double c1,c2;
    if(xMax-xMin==0) {
        c1=K1*K1;
        c2=K2*K2;
    } else {
        c1=K1*K1*(xMax-xMin)*(xMax-xMin);
        c2=K2*K2*(xMax-xMin)*(xMax-xMin);
    }
    double c3=c2/2;

    double luminance=(2*xMean*yMean+c1)/(xMean*xMean+yMean*yMean+c1);
    double contrast=(2*xSigma*ySigma+c2)/(xSigma*xSigma+ySigma*ySigma+c2);
    double structure=(xyCov+c3)/(xSigma*ySigma+c3);
    double ssim=luminance*contrast*structure;
    return ssim;
}



#endif
