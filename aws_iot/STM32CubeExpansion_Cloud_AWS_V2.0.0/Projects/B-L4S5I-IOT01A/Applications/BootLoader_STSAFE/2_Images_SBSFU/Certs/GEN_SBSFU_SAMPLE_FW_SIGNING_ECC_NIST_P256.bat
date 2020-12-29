set OPENSSL=c:\dev\OpenSSL-Win32\bin\openssl.exe

echo OFF
echo GENERATING SBSFU Sample FW Signing Keys and Certificates
echo ========================================================
echo -

echo Generating Private Key
echo ----------------------
%OPENSSL% ecparam -name prime256v1 -genkey -out SBSFU_SAMPLE_FW_SIGNING\private\sbsfu_fs_prv.pem
echo .

echo Generating Public Key
echo ---------------------
%OPENSSL% ec -in SBSFU_SAMPLE_FW_SIGNING\private\sbsfu_fs_prv.pem -pubout -out SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_pub.pem
echo .

echo Generate Certificate Signing Request
echo ------------------------------------
%OPENSSL% req -new -config SBSFU_SAMPLE_FW_SIGNING\openssl.cnf -sha256 -key SBSFU_SAMPLE_FW_SIGNING\private\sbsfu_fs_prv.pem -out SBSFU_SAMPLE_FW_SIGNING\csr\sbsfu_fs_csr.pem
echo .

echo Now go to SBSFU_SAMPLE_INTER2_CA and generate the certificate
pause

echo Generate SBSFU Sample FW Signing Test Certificate from CSR and Sign it
echo ----------------------------------------------------------------------
%OPENSSL% ca -config SBSFU_SAMPLE_INTER2_CA\openssl.cnf -extensions usr_cert -days 375 -notext -md sha256 -in SBSFU_SAMPLE_FW_SIGNING\csr\sbsfu_fs_csr.pem -out SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.pem
echo .
pause
echo Converting SBSFU Sample FW Signing Test Certificate to der format
echo -----------------------------------------------------------------
%OPENSSL% x509 -outform der -in SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.pem -out SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.der
echo .

echo SBSFU Sample FW Signing Test Certificate gererated:

%OPENSSL% x509 -in SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.pem -noout -text

copy SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.pem .
copy SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt.der .
copy SBSFU_SAMPLE_FW_SIGNING\private\sbsfu_fs_prv.pem .

echo Generate Certificate chain
type SBSFU_SAMPLE_INTER2_CA\certs\sbsfu_i2_crt.pem SBSFU_SAMPLE_INTER1_CA\certs\sbsfu_i1_crt.pem SBSFU_SAMPLE_ROOT_CA\certs\sbsfu_r0_crt.pem > SBSFU_SAMPLE_FW_SIGNING\certs\sbsfu_fs_crt_chain.pem

echo Done creating Keys and Certificates
pause
