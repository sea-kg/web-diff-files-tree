FROM php:7.4.26-apache-buster


RUN docker-php-ext-install pdo_mysql
RUN docker-php-ext-enable pdo_mysql