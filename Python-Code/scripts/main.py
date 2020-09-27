import SolarRoof as SR
import cv2 as cv
import Service

while(True):

    service = Service.Data('http://192.168.4.20')
    values = service.getData()

    if values is None:
        continue

    roof = SR.SolarRoof(200)
    roof.assign(values)
    cv.imshow('heatmap',roof.getHeatmap())
    cv.waitKey(100)