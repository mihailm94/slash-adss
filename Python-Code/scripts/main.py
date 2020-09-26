import SolarRoof as SR
import cv2 as cv
from random import randrange
import json
import requests


result = """{"variables": {
    "mode": 0,
    "val_front_0": 0,
    "val_front_1": 0,
    "val_front_2": 0,
    "val_top_0": 0,
    "val_top_1": 1000,
    "val_top_2": 0,
    "val_top_3": 0,
    "val_top_4": 0,
    "val_top_5": 0,
    "val_top_6": 0,
    "val_top_7": 0,
    "val_top_8": 0}, "id": "1", "name": "esp_data_ldr", "hardware": "esp32", "connected": true}"""

values = [100,50,100,
          200,300,200,
          10,50,20,
          0,0,0,
          0,0,0]

while(True):

    result = requests.get('http://192.168.4.20')

    if result.status_code == 200:
        result0 = result.text
    else:
        continue

    result1 = json.loads(result0)
    preVal = result1['variables']

    values = [preVal['val_front_0'], preVal['val_front_1'], preVal['val_front_2'],
              0, 0, 0,
              preVal['val_top_0'], preVal['val_top_1'], preVal['val_top_2'],
              preVal['val_top_3'], preVal['val_top_4'], preVal['val_top_5'],
              preVal['val_top_6'], preVal['val_top_7'], preVal['val_top_8']]

    roof = SR.SolarRoof(200)
    roof.assign(values)
    print(roof.getHeatmap())
    cv.imshow('heatmap',roof.getHeatmap())
    cv.waitKey(10)