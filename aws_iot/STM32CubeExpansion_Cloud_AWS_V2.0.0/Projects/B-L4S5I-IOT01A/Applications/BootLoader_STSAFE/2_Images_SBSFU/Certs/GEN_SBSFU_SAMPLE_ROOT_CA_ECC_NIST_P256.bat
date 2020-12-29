set OPENSSL=c:\dev\OpenSSL-Win32\bin\openssl.exe

echo OFF
echo GENERATING SBSFU Sample Root CA Keys and Certs
echo ==============================================
echo  

echo Generating Private Key ...
%OPENSSL% ecparam -name prime256v1 -genkey -noout -out SBSFU_SAMPLE_ROOT_CA\private\sbsfu_r0_prv.pem
echo  

echo Generating Public Key ...
%OPENSSL% ec -in SBSFU_SAMPLE_ROOT_CA\private\sbsfu_r0_prv.pem -pubout -out SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_pub.pem
echo  

echo Generate SBSFU Sample Root CA Certificate (self signed)
%OPENSSL% req -config SBSFU_SAMPLE_ROOT_CA\openssl.cnf -key SBSFU_SAMPLE_ROOT_CA\private\sbsfu_r0_prv.pem -new -x509 -days 1000 -sha256 -extensions v3_ca -out SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_crt.pem
echo  

echo Converting SBSFU Sample Root CA certificate to der format
%OPENSSL% x509 -outform der -in SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_crt.pem -out SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_crt.der
echo  

echo SBSFU Sample Root Certificate generated:

%OPENSSL% x509 -in SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_crt.pem -noout -text


echo Done