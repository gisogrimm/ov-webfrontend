# ov-webfrontend

This repository contains the code to run your own ovbox configuration interface. Two directories need to be hosted, using a `<VirtualHost name:port>` entry: the directory `user_api`, which contains the user API, and the directory `device_api`, which contains the interface for devices. Typically, both directories are hosted with and without SSL - enforcement of SSL connections is handled in the PHP code.

Install apache2, php8.3 and php8.3-xml

Add user `ov` with secure or disabled password

As user `ov`, clone repo https://github.com/gisogrimm/ov-webfrontend

Configure postfix to send emails from localhost. If needed, adjust the "From" address.

Use certbox to create SSL certificates, or install other certificates to enable https.
