/*
SQLyog Community v12.5.0 (64 bit)
MySQL - 5.7.25-0ubuntu0.18.04.2 : Database - wms_translation
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`wms_translation` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `wms_translation`;

/*Table structure for table `expressions_cache` */

DROP TABLE IF EXISTS `expressions_cache`;

CREATE TABLE `expressions_cache` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `english_expression` varchar(2000) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `expression_id` (`english_expression`,`id`) COMMENT 'expresion+id'
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

/*Data for the table `expressions_cache` */

insert  into `expressions_cache`(`id`,`english_expression`) values 
(3,'Another one to the local cache'),
(2,'I don\'t trust in google for this one'),
(1,'I need my own translation for this sentence');

/*Table structure for table `expressions_found` */

DROP TABLE IF EXISTS `expressions_found`;

CREATE TABLE `expressions_found` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `expression` varchar(250) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `expression` (`expression`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

/*Data for the table `expressions_found` */

insert  into `expressions_found`(`id`,`expression`) values 
(4,'Issue Description'),
(1,'Your Email Address');

/*Table structure for table `languages` */

DROP TABLE IF EXISTS `languages`;

CREATE TABLE `languages` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `alias` char(10) DEFAULT '',
  `language` varchar(200) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `alias` (`alias`),
  UNIQUE KEY `language` (`language`),
  UNIQUE KEY `alias_language` (`alias`,`language`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;

/*Data for the table `languages` */

insert  into `languages`(`id`,`alias`,`language`) values 
(1,'auto','Automatic'),
(4,'de','German'),
(2,'en','English'),
(3,'es','Spanish'),
(5,'pt','Portuguese');

/*Table structure for table `translations` */

DROP TABLE IF EXISTS `translations`;

CREATE TABLE `translations` (
  `expression_id` int(11) DEFAULT '0' COMMENT 'ID from expressionsCache table',
  `to_language_id` int(11) DEFAULT '0' COMMENT 'ID from languages table',
  `translation` varchar(2000) DEFAULT '' COMMENT 'Expression translated FROM english INTO toLanguageID language',
  UNIQUE KEY `UniqeTranslat` (`expression_id`,`to_language_id`) COMMENT 'Only one translation per expression/language'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Data for the table `translations` */

insert  into `translations`(`expression_id`,`to_language_id`,`translation`) values 
(1,3,'Quiero tener mi propia traduccion de esta oracion'),
(2,3,'No voy a confiar en google para este caso'),
(3,3,'Otra para traducion local'),
(1,4,'Ich brauche meine Übersetzung für diesen Satz');

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
