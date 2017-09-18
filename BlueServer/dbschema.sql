CREATE TABLE `BattleLog` (
  `lid` bigint(20) NOT NULL AUTO_INCREMENT,
  `uid` bigint(20) DEFAULT NULL,
  `battleLog` blob,
  `regDate` datetime DEFAULT NULL,
  PRIMARY KEY (`lid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `CharData` (
  `cid` bigint(20) NOT NULL AUTO_INCREMENT,
  `uid` bigint(20) NOT NULL,
  `slotNo` int(11) NOT NULL,
  `typeNo` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `tier` int(11) NOT NULL,
  `regDate` datetime NOT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `DungeonData` (
  `uid` bigint(20) NOT NULL,
  `dungeonNo` int(11) NOT NULL,
  `dungeonTier` int(11) NOT NULL,
  `updateDate` datetime DEFAULT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `UserData` (
  `uid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL,
  `did` varchar(64) NOT NULL,
  `platform` int(11) NOT NULL,
  `loginDate` datetime DEFAULT NULL,
  `logoutDate` datetime DEFAULT NULL,
  `regDate` datetime DEFAULT NULL,
  `vc1` int(11) NOT NULL,
  `vc2` int(11) NOT NULL,
  `vc3` int(11) NOT NULL,
  `groupName` varchar(64) DEFAULT ' ',
  `language` varchar(64) DEFAULT ' ',
  PRIMARY KEY (`uid`),
  KEY `INDEX_NAME` (`name`,`uid`)
) ENGINE=InnoDB AUTO_INCREMENT=1142 DEFAULT CHARSET=utf8;
