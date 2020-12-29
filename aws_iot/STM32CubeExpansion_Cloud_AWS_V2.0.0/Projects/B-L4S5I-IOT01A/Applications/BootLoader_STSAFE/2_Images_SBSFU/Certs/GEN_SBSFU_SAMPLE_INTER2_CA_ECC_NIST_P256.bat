set OPENSSL=c:\dev\OpenSSL-Win32\bin\openssl.exe

echo OFF
echo GENERATING SBSFU Sample Intermediate 2 CA Keys and Certificates
echo ===============================================================
echo -

echo Generating Private Key
echo ----------------------
%OPENSSL% ecparam -name prime256v1 -genkey -out SBSFU_SAMPLE_INTER2_CA\private\sbsfu_i2_prv.pem
echo .

echo Generating Public Key
echo ---------------------
%OPENSSL% ec -in SBSFU_SAMPLE_INTER2_CA\private\sbsfu_i2_prv.pem -pubout -out SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_pub.pem
echo .

echo Generate Certificate Signing Request
echo ------------------------------------
%OPENSSL% req -new -config SBSFU_SAMPLE_INTER2_CA\openssl.cnf -sha256 -key SBSFU_SAMPLE_INTER2_CA\private\sbsfu_i2_prv.pem -out SBSFU_SAMPLE_INTER2_CA\csr\sbsfu_i2_csr.pem
echo .

echo Now go to Root CA and generate the certificate
pause

echo Generate SBSFU Sample Intermediate 1 CA Certificate from CSR and Sign it
echo ------------------------------------------------------------------------
%OPENSSL% ca -config SBSFU_SAMPLE_INTER1_CA\openssl.cnf -extensions v3_intermediate_ca -days 375 -notext -md sha256 -in SBSFU_SAMPLE_INTER2_CA\csr\sbsfu_i2_csr.pem -out SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.pem
echo .

echo Converting SBSFU Sample Intermediate 1 CA Certificate to der format
echo -------------------------------------------------------------------
%OPENSSL% x509 -outform der -in SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.pem -out SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.der
echo .

echo SBSFU Sample Intermediate 1 CA Certificate generated:

%OPENSSL% x509 -in SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.pem -noout -text

copy SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.pem .
copy SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.der .

echo Done creating SBSFU Sample Intermediate 1 CA Keys and Certificates
pause