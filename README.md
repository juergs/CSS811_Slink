# CSS811_Slink

Using CSS811-Breakoutboard: CJMCU-811.

Output of CSS811 is CO2, TVOC and temperature   mapped to BME680-SLINK-UDP-Transmission, in order to 
get these readings in [FHEM](https://forum.fhem.de/index.php). 

Using these additional modules: https://github.com/herrmannj/AirQuality/tree/master/FHEM.
This definition will be needed with autocreate set to true:
``
define slink slink 
```

![CSS_WemosD1](https://github.com/juergs/CSS811_Slink/blob/master/Wemos_D1_CSS811.png)

![CSS_WemosD1_PLOT](https://github.com/juergs/CSS811_Slink/blob/master/Wemos_D1_CSS811_Plot.png)


Mapping sketch output to BME680-UDP-transfer-protocol: SLINK

```
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_temperature: 88.16
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_rH: 0.00
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_aH: 0.00
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_dewpoint: 0.00
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_pressure: 0.0
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_tvoc: 640
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_DBG_R: 1073676738
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_DBG_BASE_C: 1073676720
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_DBG_FILTERED: 38
2019-11-03 16:49:34 SLinkIAQC SLinkIAQC_633D09 BME680_DBG_RATIO: 1
```


![CSS_WemosD1_PLOT](https://github.com/juergs/CSS811_Slink/blob/master/Wemos_D1_CSS811_Fhem.png)
