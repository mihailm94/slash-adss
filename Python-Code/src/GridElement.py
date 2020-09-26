import numpy as np

class GridElement:

    def __init__(self, value ,subgrid=None):

        self.value = value

        if subgrid is None:
            self.subgrid = []
        else:
            self.subgrid = self.createGrid(subgrid,value)

        self.dictionary = {
            'ul': 0,
            'uc': 1,
            'ur': 2,
            'cl': 3,
            'cc': 4,
            'cr': 5,
            'll': 6,
            'lc': 7,
            'lr': 8
        }
    def setNeighbour(self,direction, value):
        """

        :param direction:
        ul: upper left 0
        uc: upper center 1
        ur: upper right 2

        cl: center left 3
        cc: center center 4
        cr: center right 5

        ll: lower left 6
        lc: lower center 7
        lr: lower right 8
        :return:
        """

        if(direction[1] == 'c'):
            self.__setGrid(direction,value)
        else:
            self.__setGrid(direction, value*0.5)
    def __setGrid(self,direction, value):
        self.subgrid[self.dictionary[direction]] = abs((self.value - value) / 2)
    def createGrid(self, dim, default_value=0):
        grids = []
        for i in range(dim*dim):
            grids.append(default_value)

        self.subgrid = grids
        self.subgridDim = dim




class Grid:

    def __init__(self, grid):
        self.gridElements = grid


    def drawCell(self, element, size):

        if len(element.subgridDim) > 1:
            subsize = int(size/element.subgridDim)
            grid = None
            vgrid = None
            for h in range(element.subgridDim):
                for w in range(element.subgridDim):
                    if grid is None:
                        grid = np.ones((subsize,subsize))*element.value
                    else:
                        grid = np.hstack(grid,np.ones((subsize,subsize))*element.value)
                if vgrid is None:
                    vgrid = grid
                else:
                    vgrid = np.vstack(vgrid,grid)

            finalgrid = vgrid

        else:
            finalgrid = np.ones((size,size))*element.value


        return finalgrid








