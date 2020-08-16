
DROP TABLE IF EXISTS `guild_bbs_replies`;
CREATE TABLE `guild_bbs_replies` (
  `reply_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `thread_id` int(10) unsigned NOT NULL,
  `poster_char_id` int(10) unsigned NOT NULL,
  `timestamp` bigint(20) unsigned NOT NULL,
  `content` varchar(26) NOT NULL DEFAULT '',
  PRIMARY KEY (`reply_id`)
);

DROP TABLE IF EXISTS `guild_bbs_threads`;
CREATE TABLE `guild_bbs_threads` (
  `thread_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `poster_char_id` int(10) unsigned NOT NULL,
  `name` varchar(26) NOT NULL DEFAULT '',
  `timestamp` bigint(20) unsigned NOT NULL,
  `icon` smallint(5) unsigned NOT NULL,
  `reply_count` smallint(5) unsigned NOT NULL DEFAULT '0',
  `startpost` text NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `local_thread_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`thread_id`)
);
