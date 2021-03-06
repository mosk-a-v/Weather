CREATE TABLE `ControlTable` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Sun` int(11) NOT NULL DEFAULT '0',
  `Wind` int(11) NOT NULL DEFAULT '0',
  `Indoor` float NOT NULL,
  `Outdoor` float NOT NULL,
  `Boiler` float NOT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=4120 DEFAULT CHARSET=latin1;

CREATE TABLE `CycleStatistics` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `CycleStart` datetime DEFAULT NULL,
  `BoilerRequired` float DEFAULT NULL,
  `IsHeating` bit(1) DEFAULT NULL,
  `CycleLength` int(11) DEFAULT NULL,
  `Result` smallint(6) DEFAULT NULL,
  `MaxDelta` float DEFAULT NULL,
  `Delta` float DEFAULT NULL,
  `AvgRadioOutdoor` float DEFAULT NULL,
  `LastRadioOutdoor` float DEFAULT NULL,
  `AvgRadioBoiler` float DEFAULT NULL,
  `LastRadioBoiler` float DEFAULT NULL,
  `AvgRadioBedroom` float DEFAULT NULL,
  `LastRadioBedroom` float DEFAULT NULL,
  `AvgRadioLounge` float DEFAULT NULL,
  `LastRadioLounge` float DEFAULT NULL,
  `AvgRadioMansard` float DEFAULT NULL,
  `LastRadioMansard` float DEFAULT NULL,
  `AvgRadioStudy` float DEFAULT NULL,
  `LastRadioStudy` float DEFAULT NULL,
  `AvgDirectBoiler` float DEFAULT NULL,
  `LastDirectBoiler` float DEFAULT NULL,
  `AvgDirectIndoor` float DEFAULT NULL,
  `LastDirectIndoor` float DEFAULT NULL,
  `AvgDirectOtdoor` float DEFAULT NULL,
  `LastDirectOtdoor` float DEFAULT NULL,
  `GlobalSun` float DEFAULT NULL,
  `GlobalWind` float DEFAULT NULL,
  `GlobalOutdoor` float DEFAULT NULL,
  `LoungeHumidity` float DEFAULT NULL,
  `MansardHumidity` float DEFAULT NULL,
  `StudyHumidity` float DEFAULT NULL,
  `AvgRadioKitchen` float DEFAULT NULL,
  `LastRadioKitchen` float DEFAULT NULL,
  `KitchenHumidity` float DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2355 DEFAULT CHARSET=latin1;

CREATE TABLE TemperatureSettings (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `WeekDay` int(11) DEFAULT NULL,
  `Hour` int(11) DEFAULT NULL,
  `Temperature` float DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=1513 DEFAULT CHARSET=latin1;

CREATE TABLE `SensorTable` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `SensorId` int(11) NOT NULL,
  `SensorIdentifier` varchar(20) NOT NULL,
  `UseForCalc` tinyint(4) DEFAULT NULL,
  `IsIndoor` tinyint(4) DEFAULT NULL,
  `IsOutdoor` tinyint(4) DEFAULT NULL,
  `IsDirect` tinyint(4) DEFAULT NULL,
  `CorrectionCoefficient` float NOT NULL DEFAULT '1',
  `Shift` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=4100 DEFAULT CHARSET=latin1;

delete FROM ControlTable;
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, 20, 20);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, 10, 28);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, 0, 36);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, -10, 42);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, -20, 48);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (20, -30, 54);

INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, 20, 18);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, 10, 25);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, 0, 33);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, -10, 39);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, -20, 45);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (18, -30, 51);

INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, 20, 20);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, 10, 31);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, 0, 40);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, -10, 45);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, -20, 51);
INSERT INTO ControlTable (`Indoor`, `Outdoor`, `Boiler`) VALUES (22, -30, 57);

delete FROM SensorTable;
INSERT INTO SensorTable VALUES (4090,1,'113',1,0,1,0,1,0);
INSERT INTO SensorTable VALUES (4091,2,'142',1,1,0,0,1,0);
INSERT INTO SensorTable VALUES (4092,3,'179',0,0,0,0,1,0);
INSERT INTO SensorTable VALUES (4093,4,'69',0,1,0,0,1,0);
INSERT INTO SensorTable VALUES (4094,5,'134',0,1,0,0,1,0);
INSERT INTO SensorTable VALUES (4095,6,'12',1,1,0,0,1,0);
INSERT INTO SensorTable VALUES (4096,7,'030797794e9d',0,0,0,1,1,1);
INSERT INTO SensorTable VALUES (4097,8,'03069779b057',1,1,0,1,1,0.4);
INSERT INTO SensorTable VALUES (4098,9,'01144ab139aa',1,0,1,1,1,0.6);
INSERT INTO SensorTable VALUES (4099,10,'105',0,1,0,0,1,0);

delete FROM TemperatureSettings;
INSERT INTO TemperatureSettings VALUES (1345,1,0,20);
INSERT INTO TemperatureSettings VALUES (1346,1,1,20);
INSERT INTO TemperatureSettings VALUES (1347,1,2,20);
INSERT INTO TemperatureSettings VALUES (1348,1,3,20);
INSERT INTO TemperatureSettings VALUES (1349,1,4,20);
INSERT INTO TemperatureSettings VALUES (1350,1,5,20);
INSERT INTO TemperatureSettings VALUES (1351,1,6,20);
INSERT INTO TemperatureSettings VALUES (1352,1,7,20);
INSERT INTO TemperatureSettings VALUES (1353,1,8,20);
INSERT INTO TemperatureSettings VALUES (1354,1,9,20);
INSERT INTO TemperatureSettings VALUES (1355,1,10,20);
INSERT INTO TemperatureSettings VALUES (1356,1,11,20);
INSERT INTO TemperatureSettings VALUES (1357,1,12,20);
INSERT INTO TemperatureSettings VALUES (1358,1,13,20);
INSERT INTO TemperatureSettings VALUES (1359,1,14,20);
INSERT INTO TemperatureSettings VALUES (1360,1,15,20);
INSERT INTO TemperatureSettings VALUES (1361,1,16,20);
INSERT INTO TemperatureSettings VALUES (1362,1,17,20);
INSERT INTO TemperatureSettings VALUES (1363,1,18,20);
INSERT INTO TemperatureSettings VALUES (1364,1,19,20);
INSERT INTO TemperatureSettings VALUES (1365,1,20,20);
INSERT INTO TemperatureSettings VALUES (1366,1,21,20);
INSERT INTO TemperatureSettings VALUES (1367,1,22,20);
INSERT INTO TemperatureSettings VALUES (1368,1,23,20);
INSERT INTO TemperatureSettings VALUES (1369,2,0,20);
INSERT INTO TemperatureSettings VALUES (1370,2,1,20);
INSERT INTO TemperatureSettings VALUES (1371,2,2,20);
INSERT INTO TemperatureSettings VALUES (1372,2,3,20);
INSERT INTO TemperatureSettings VALUES (1373,2,4,20);
INSERT INTO TemperatureSettings VALUES (1374,2,5,20);
INSERT INTO TemperatureSettings VALUES (1375,2,6,20);
INSERT INTO TemperatureSettings VALUES (1376,2,7,20);
INSERT INTO TemperatureSettings VALUES (1377,2,8,20);
INSERT INTO TemperatureSettings VALUES (1378,2,9,20);
INSERT INTO TemperatureSettings VALUES (1379,2,10,20);
INSERT INTO TemperatureSettings VALUES (1380,2,11,20);
INSERT INTO TemperatureSettings VALUES (1381,2,12,20);
INSERT INTO TemperatureSettings VALUES (1382,2,13,20);
INSERT INTO TemperatureSettings VALUES (1383,2,14,20);
INSERT INTO TemperatureSettings VALUES (1384,2,15,20);
INSERT INTO TemperatureSettings VALUES (1385,2,16,20);
INSERT INTO TemperatureSettings VALUES (1386,2,17,20);
INSERT INTO TemperatureSettings VALUES (1387,2,18,20);
INSERT INTO TemperatureSettings VALUES (1388,2,19,20);
INSERT INTO TemperatureSettings VALUES (1389,2,20,20);
INSERT INTO TemperatureSettings VALUES (1390,2,21,20);
INSERT INTO TemperatureSettings VALUES (1391,2,22,20);
INSERT INTO TemperatureSettings VALUES (1392,2,23,20);
INSERT INTO TemperatureSettings VALUES (1393,3,0,20);
INSERT INTO TemperatureSettings VALUES (1394,3,1,20);
INSERT INTO TemperatureSettings VALUES (1395,3,2,20);
INSERT INTO TemperatureSettings VALUES (1396,3,3,20);
INSERT INTO TemperatureSettings VALUES (1397,3,4,20);
INSERT INTO TemperatureSettings VALUES (1398,3,5,20);
INSERT INTO TemperatureSettings VALUES (1399,3,6,20);
INSERT INTO TemperatureSettings VALUES (1400,3,7,20);
INSERT INTO TemperatureSettings VALUES (1401,3,8,20);
INSERT INTO TemperatureSettings VALUES (1402,3,9,20);
INSERT INTO TemperatureSettings VALUES (1403,3,10,20);
INSERT INTO TemperatureSettings VALUES (1404,3,11,20);
INSERT INTO TemperatureSettings VALUES (1405,3,12,20);
INSERT INTO TemperatureSettings VALUES (1406,3,13,20);
INSERT INTO TemperatureSettings VALUES (1407,3,14,20);
INSERT INTO TemperatureSettings VALUES (1408,3,15,20);
INSERT INTO TemperatureSettings VALUES (1409,3,16,20);
INSERT INTO TemperatureSettings VALUES (1410,3,17,20);
INSERT INTO TemperatureSettings VALUES (1411,3,18,20);
INSERT INTO TemperatureSettings VALUES (1412,3,19,20);
INSERT INTO TemperatureSettings VALUES (1413,3,20,20);
INSERT INTO TemperatureSettings VALUES (1414,3,21,20);
INSERT INTO TemperatureSettings VALUES (1415,3,22,20);
INSERT INTO TemperatureSettings VALUES (1416,3,23,20);
INSERT INTO TemperatureSettings VALUES (1417,4,0,20);
INSERT INTO TemperatureSettings VALUES (1418,4,1,20);
INSERT INTO TemperatureSettings VALUES (1419,4,2,20);
INSERT INTO TemperatureSettings VALUES (1420,4,3,20);
INSERT INTO TemperatureSettings VALUES (1421,4,4,20);
INSERT INTO TemperatureSettings VALUES (1422,4,5,20);
INSERT INTO TemperatureSettings VALUES (1423,4,6,20);
INSERT INTO TemperatureSettings VALUES (1424,4,7,20);
INSERT INTO TemperatureSettings VALUES (1425,4,8,20);
INSERT INTO TemperatureSettings VALUES (1426,4,9,20);
INSERT INTO TemperatureSettings VALUES (1427,4,10,20);
INSERT INTO TemperatureSettings VALUES (1428,4,11,20);
INSERT INTO TemperatureSettings VALUES (1429,4,12,20);
INSERT INTO TemperatureSettings VALUES (1430,4,13,20);
INSERT INTO TemperatureSettings VALUES (1431,4,14,20);
INSERT INTO TemperatureSettings VALUES (1432,4,15,20);
INSERT INTO TemperatureSettings VALUES (1433,4,16,20);
INSERT INTO TemperatureSettings VALUES (1434,4,17,20);
INSERT INTO TemperatureSettings VALUES (1435,4,18,20);
INSERT INTO TemperatureSettings VALUES (1436,4,19,20);
INSERT INTO TemperatureSettings VALUES (1437,4,20,20);
INSERT INTO TemperatureSettings VALUES (1438,4,21,20);
INSERT INTO TemperatureSettings VALUES (1439,4,22,20);
INSERT INTO TemperatureSettings VALUES (1440,4,23,20);
INSERT INTO TemperatureSettings VALUES (1441,5,0,20);
INSERT INTO TemperatureSettings VALUES (1442,5,1,20);
INSERT INTO TemperatureSettings VALUES (1443,5,2,20);
INSERT INTO TemperatureSettings VALUES (1444,5,3,20);
INSERT INTO TemperatureSettings VALUES (1445,5,4,20);
INSERT INTO TemperatureSettings VALUES (1446,5,5,20);
INSERT INTO TemperatureSettings VALUES (1447,5,6,20);
INSERT INTO TemperatureSettings VALUES (1448,5,7,20);
INSERT INTO TemperatureSettings VALUES (1449,5,8,20);
INSERT INTO TemperatureSettings VALUES (1450,5,9,20);
INSERT INTO TemperatureSettings VALUES (1451,5,10,20);
INSERT INTO TemperatureSettings VALUES (1452,5,11,20);
INSERT INTO TemperatureSettings VALUES (1453,5,12,20);
INSERT INTO TemperatureSettings VALUES (1454,5,13,20);
INSERT INTO TemperatureSettings VALUES (1455,5,14,20);
INSERT INTO TemperatureSettings VALUES (1456,5,15,20);
INSERT INTO TemperatureSettings VALUES (1457,5,16,20);
INSERT INTO TemperatureSettings VALUES (1458,5,17,20);
INSERT INTO TemperatureSettings VALUES (1459,5,18,20);
INSERT INTO TemperatureSettings VALUES (1460,5,19,20);
INSERT INTO TemperatureSettings VALUES (1461,5,20,20);
INSERT INTO TemperatureSettings VALUES (1462,5,21,20);
INSERT INTO TemperatureSettings VALUES (1463,5,22,20);
INSERT INTO TemperatureSettings VALUES (1464,5,23,20);
INSERT INTO TemperatureSettings VALUES (1465,6,0,20);
INSERT INTO TemperatureSettings VALUES (1466,6,1,20);
INSERT INTO TemperatureSettings VALUES (1467,6,2,20);
INSERT INTO TemperatureSettings VALUES (1468,6,3,20);
INSERT INTO TemperatureSettings VALUES (1469,6,4,20);
INSERT INTO TemperatureSettings VALUES (1470,6,5,20);
INSERT INTO TemperatureSettings VALUES (1471,6,6,20);
INSERT INTO TemperatureSettings VALUES (1472,6,7,20);
INSERT INTO TemperatureSettings VALUES (1473,6,8,20);
INSERT INTO TemperatureSettings VALUES (1474,6,9,20);
INSERT INTO TemperatureSettings VALUES (1475,6,10,20);
INSERT INTO TemperatureSettings VALUES (1476,6,11,20);
INSERT INTO TemperatureSettings VALUES (1477,6,12,20);
INSERT INTO TemperatureSettings VALUES (1478,6,13,20);
INSERT INTO TemperatureSettings VALUES (1479,6,14,20);
INSERT INTO TemperatureSettings VALUES (1480,6,15,20);
INSERT INTO TemperatureSettings VALUES (1481,6,16,20);
INSERT INTO TemperatureSettings VALUES (1482,6,17,20);
INSERT INTO TemperatureSettings VALUES (1483,6,18,20);
INSERT INTO TemperatureSettings VALUES (1484,6,19,20);
INSERT INTO TemperatureSettings VALUES (1485,6,20,20);
INSERT INTO TemperatureSettings VALUES (1486,6,21,20);
INSERT INTO TemperatureSettings VALUES (1487,6,22,20);
INSERT INTO TemperatureSettings VALUES (1488,6,23,20);
INSERT INTO TemperatureSettings VALUES (1489,7,0,20);
INSERT INTO TemperatureSettings VALUES (1490,7,1,20);
INSERT INTO TemperatureSettings VALUES (1491,7,2,20);
INSERT INTO TemperatureSettings VALUES (1492,7,3,20);
INSERT INTO TemperatureSettings VALUES (1493,7,4,20);
INSERT INTO TemperatureSettings VALUES (1494,7,5,20);
INSERT INTO TemperatureSettings VALUES (1495,7,6,20);
INSERT INTO TemperatureSettings VALUES (1496,7,7,20);
INSERT INTO TemperatureSettings VALUES (1497,7,8,20);
INSERT INTO TemperatureSettings VALUES (1498,7,9,20);
INSERT INTO TemperatureSettings VALUES (1499,7,10,20);
INSERT INTO TemperatureSettings VALUES (1500,7,11,20);
INSERT INTO TemperatureSettings VALUES (1501,7,12,20);
INSERT INTO TemperatureSettings VALUES (1502,7,13,20);
INSERT INTO TemperatureSettings VALUES (1503,7,14,20);
INSERT INTO TemperatureSettings VALUES (1504,7,15,20);
INSERT INTO TemperatureSettings VALUES (1505,7,16,20);
INSERT INTO TemperatureSettings VALUES (1506,7,17,20);
INSERT INTO TemperatureSettings VALUES (1507,7,18,20);
INSERT INTO TemperatureSettings VALUES (1508,7,19,20);
INSERT INTO TemperatureSettings VALUES (1509,7,20,20);
INSERT INTO TemperatureSettings VALUES (1510,7,21,20);
INSERT INTO TemperatureSettings VALUES (1511,7,22,20);
INSERT INTO TemperatureSettings VALUES (1512,7,23,20);
