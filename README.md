# Z-checker
a library to characterize the data and check the compression results of lossy compressors.

**We strongly recommend QCAT (https://github.com/szcompressor/qcat), a lightweight variant of Z-checker. Each metric is implemented as both a standalone executable and an API call.**

Z-checker-intaller can help you install everything (including compressors SZ and ZFP and other third-party libraries for plotting the figures). Z-checker-installer can be downloaded from http://github.com/CODARcode/z-checker-installer. Please read the user-guide of Z-checker for details.

This version supports CPU and MPI. If you are interested in the GPU version of Z-checker, please use cuZ-checker (https://github.com/CODARcode/cuZ-checker)

For quick data compression toolkit, you can use the qcat tool, which is included in this package. It can help you conveniently conduct data type conversion, file format conversion and quick data visualization, and compare the original data and decompressed data simply. The qcat library can also be found on github (https://github.com/szcompressor/qcat.git).

Please cite the following paper if you used Z-checker in your research/papers. Thanks!

Tao, D., Di, S., Guo, H., Chen, Z., & Cappello, F. (2019). Z-checker: A framework for assessing lossy compression of scientific data. The International Journal of High Performance Computing Applications, 33(2), 285–303. 
https://journals.sagepub.com/doi/abs/10.1177/1094342017737147


Compressors: 
* SZ (autotuning): Kai Zhao, Sheng Di, Xin Liang, Sihuan Li, Dingwen Tao, Zizhong Chen, and Franck Cappello. "Significantly Improving Lossy Compression for HPC Datasets with Second-Order Prediction and Parameter Optimization", Proceedings of the 29th International Symposium on High-Performance Parallel and Distributed Computing (HPDC 20), Stockholm, Sweden, 2020. (Code: https://github.com/lxAltria/meta_compressor/tree/autotuning)

* SZ 2.0+: Xin Liang, Sheng Di, Dingwen Tao, Zizhong Chen, Franck Cappello, "[Error-Controlled Lossy Compression Optimized for High Compression Ratios of Scientific Datasets](https://ieeexplore.ieee.org/document/8622520)", in IEEE International Conference on Big Data (Bigdata 2018), Seattle, WA, USA, 2018.

* SZ 1.4.0-1.4.13: Dingwen Tao, Sheng Di, Franck Cappello, "[Significantly Improving Lossy Compression for Scientific Data Sets Based on Multidimensional Prediction and Error-Controlled Quantization](https://ieeexplore.ieee.org/document/7967203)", in IEEE International Parallel and Distributed Processing Symposium (IPDPS 2017), Orlando, Florida, USA, 2017.

* SZ 0.1-1.0: Sheng Di, Franck Cappello, "[Fast Error-bounded Lossy HPC Data Compression with SZ](https://ieeexplore.ieee.org/document/7516069)", in IEEE International Parallel and Distributed Processing Symposium (IPDPS 2016), Chicago, IL, USA, 2016.
