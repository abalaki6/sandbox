import numpy as np


def create_sphere(R, r, X, Y ,Z):
    data = np.zeros((X,Y,Z))

    xs,ys,zs = np.linspace(-.5,0.5,X), np.linspace(-0.5,0.5,Y), np.linspace(-0.5,0.5,Z)

    for i,x in enumerate(xs):
        for j,y in enumerate(ys):
            for k,z in enumerate(zs):
                d = x**2 + y**2 + z**2
                if(d > r**2 and d < R**2):
                    data[i,j,k] = 3.0
                else:
                    data[i,j,k] = 0.0
    
    data.flatten().astype(np.double).tofile("sphere_grid.bin")


if __name__ == "__main__":
    dx, dy, dz = [1e-2]*3
    create_sphere(0.3, 0.1, int(1/dx) + 1, int(1/dy) + 1, int(1/dz) + 1)