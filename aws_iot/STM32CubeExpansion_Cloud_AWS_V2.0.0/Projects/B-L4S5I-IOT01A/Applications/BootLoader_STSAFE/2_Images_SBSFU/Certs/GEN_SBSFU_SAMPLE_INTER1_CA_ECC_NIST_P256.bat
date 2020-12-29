set OPENSSL=c:\dev\OpenSSL-Win32\bin\openssl.exe

echo OFF
echo GENERATING SBSFU Sample Intermediate 1 CA Keys and Certificate
echo ==============================================================
echo -

echo Generating Private Key
echo ----------------------
%OPENSSL% ecparam -name prime256v1 -genkey -out SBSFU_SAMPLE_INTER1_CA\private\sbsfu_i1_prv.pem
echo .

echo Generating Public Key
echo ---------------------
%OPENSSL% ec -in SBSFU_SAMPLE_INTER1_CA\private\sbsfu_i1_prv.pem -pubout -out SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_pub.pem
echo .

echo Generate Certificate Signing Request
echo ------------------------------------
%OPENSSL% req -new -config SBSFU_SAMPLE_INTER1_CA\openssl.cnf -sha256 -key SBSFU_SAMPLE_INTER1_CA\private\sbsfu_i1_prv.pem -out SBSFU_SAMPLE_INTER1_CA\csr\sbsfu_i1_csr.pem
echo .

echo Now go to Root CA and generate the certificate
pause

echo Generate Intermediate 1 Certificate from CSR and Sign it
echo --------------------------------------------------------
%OPENSSL% ca -config SBSFU_SAMPLE_ROOT_CA\openssl.cnf -extensions v3_intermediate_ca -days 375 -notext -md sha256 -in SBSFU_SAMPLE_INTER1_CA\csr\sbsfu_i1_csr.pem -out SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_crt.pem
echo .

echo Converting Intermediate 1 certificate to der format
echo ---------------------------------------------------
%OPENSSL% x509 -outform der -in SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_crt.pem -out SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_crt.der
echo .

echo Intermediate 1 Certificate generated:

%OPENSSL% x509 -in SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_crt.pem -noout -text

echo Done creating Intermediate 1 keys and Certificates
pause