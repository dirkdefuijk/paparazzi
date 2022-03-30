import numpy as np
import matplotlib.pylab as plt
import csv
import seaborn as sb
import math 

def load_position(filename):
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        pos_x = np.zeros((215591)) # no. datapoints
        pos_z = np.zeros((215591))
        time = np.zeros((215591))
        for row in reader:
            pos_x[int(row["Frame"])] = float(row['pos_X'])
            pos_z[int(row["Frame"])] = float(row['pos_Z'])
            time[int(row["Frame"])] = float(row['Time'])
    return pos_x, pos_z, time

def plot_traj(pos_x, pos_z):
    plt.figure()
    plt.plot(pos_x,pos_z)
    plt.grid()
    plt.xlabel("position x [m]")
    plt.ylabel("position z [m]")
    plt.show()


def heatmap(pos_x, pos_z,time):
    axes = np.zeros((8,8))
    velocity= np.zeros(len(pos_x))
    
    for i, x_l in enumerate(np.arange(-4, 4)):
        x_u = x_l + 1

        for j, z_l in enumerate(np.arange(-4, 4)):
            z_u = z_l + 1
                
            for z in range(1, len(pos_x) ):
                if (pos_x[i] < x_u and pos_z[i] < z_u) and (pos_x[i] >= x_l and pos_z[i] >= z_l):
                    dx = pos_x[z] - pos_x[z-1]
                    dz = pos_z[z] - pos_z[z-1]
                    distance = math.sqrt(dx**2 + dz**2)
                    dT = time[z]-time[z-1]
                    velocity[z] = distance/ dT #velocity compared to previous datapoint
            
            hits = np.multiply(pos_x >= x_l, pos_x < x_u)
            hits = np.multiply(pos_z >= z_l, hits)
            hits = np.multiply(pos_z  < z_u, hits)

            if sum(velocity) !=0 and sum(hits)!=0:
                velo_AVG = sum(velocity) / sum(hits)
            else:
                velo_AVG = 0
            axes[i,j] = velo_AVG
        
    ax = sb.heatmap(np.transpose(axes))
    ax.invert_yaxis()
    ax.plot(pos_x+3,pos_z+3)
    plt.show()


def main():
    pos_x, pos_z,time = load_position("group_2.csv")
    heatmap(pos_x,pos_z,time)
    



if __name__ == "__main__":
    main()




