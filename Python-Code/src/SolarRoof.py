import cv2 as cv
import numpy as np
import GridElement as Grid

class SolarRoof:

    def __init__(self,gridSize):

        self.gridSize = gridSize
        self.container = np.zeros((self.gridSize*5+2,self.gridSize*3+2))
        print(self.container.shape)
        self.grid = []

    def assign(self,values,subgrid=3):

        if len(values) != 15:
            raise ValueError("That's a problem, uneven values!")

        new_values = self.__scaleValues(values)
        H = self.container.shape[0]
        W = self.container.shape[1]

        for i in range(len(values)-1):
            self.grid.append(Grid.GridElement(values[i]))

        for i in range(0,H-2):
            for j in range(0,W-2):
                v = int(i / self.gridSize)
                w = int(j / self.gridSize)

                if w < 1:
                    self.container[i+1,j+1] = new_values[v*3+w]
                elif w < 2:
                    self.container[i+1, j+1] = new_values[v*3+w]
                elif w < 3:
                    self.container[i+1, j+1] = new_values[v*3+w]
                elif w < 4:
                    self.container[i+1, j+1] = new_values[v*3+w]
                elif w < 5:
                    self.container[i + 1, j + 1] = new_values[v * 3 + w]



    def __scaleValues(self,values):
        maxV = max(values)
        newList_one = values.copy()
        zeros = np.array(values) < 0
        zeros = np.where(zeros)

        if len(zeros[0]) > 0:
         newList_one[zeros] = 0

        if maxV != 0:
            newList_one = list(map(lambda x: x/maxV, newList_one))
        else:
            newList_one = list(map(lambda x: 0, newList_one))


        return newList_one


    def getHeatmap(self):
        norm = cv.normalize(self.container,None,alpha=0,beta=255, norm_type = cv.NORM_MINMAX, dtype = cv.CV_32F)
        return  cv.applyColorMap(norm.astype(np.uint8), cv.COLORMAP_JET)

