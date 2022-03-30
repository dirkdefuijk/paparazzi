import multiprocessing
import numpy as np
import matplotlib.pylab as plt
import csv
import numpy as np; np.random.seed(0)
import seaborn as sns; sns.set_theme()
import scipy
import os.path
import math

from multiprocessing import Pool, freeze_support
from functools import partial
from itertools import repeat, starmap
from itertools import starmap
from multiprocessing import Process

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
    plt.title("Trajectory")
    plt.show()
    plt.savefig("trajectory.png", dpi=300)

def discritizer_pos(pos_x,pos_z):
    N = 8 #20
    grid_XY = np.zeros([N,N])
    n_indx = 0
    
    for i, x_l in enumerate(np.arange(-3, 5)):
        x_u = x_l + 1

        for j, z_l in enumerate(np.arange(-3, 5)):
            z_u = z_l + 1

            hits = np.multiply(pos_x >= x_l, pos_x < x_u)
            hits = np.multiply(pos_z >= z_l, hits)
            hits = np.multiply(pos_z  < z_u, hits)

            grid_XY[i,j] = sum(hits)
    # for n in range(-3,5):
    #     m_indx = 0
    #     for m in range(-3,5):
    #         for i in range(len(pos_x)):
    #             if (pos_x[i] < n and pos_z[i] < m) and (pos_x[i] >= n-1 and pos_z[i] >= m-1):
    #                 grid_XY[n_indx,m_indx] += 1
    #         m_indx += 1
    #     n_indx += 1
    for n_indx in range(0,8):
        for m_indx in range(0,8):
            grid_XY[n_indx,m_indx] = (grid_XY[n_indx,m_indx] / len(pos_x)) *100
    return np.transpose(grid_XY)

def parallel_handler(pos_x, pos_z, time, n, m):
    # velo = 0
    # grid_count = 0
    x_l = n
    x_u = x_l + 1
    z_l = m
    z_u = z_l + 1
    velocity= np.zeros(len(pos_x))

    for i in range(1, len(pos_x) ):
        dx = pos_x[i] - pos_x[i-1]
        dz = pos_z[i] - pos_z[i-1]
        distance = math.sqrt(dx**2 + dz**2)
        dT = time[i]-time[i-1]
        velocity[i] = distance/ dT #velocity compared to previous datapoint

    hits = np.multiply(pos_x >= x_l, pos_x < x_u)
    hits = np.multiply(pos_z >= z_l, hits)
    hits = np.multiply(pos_z  < z_u, hits)

    if sum(velocity) !=0 and sum(hits)!=0:
        velo_AVG = sum(velocity) / sum(hits)
    else:
        velo_AVG = 0

    # for i in range(1, len(pos_x) ):
    #     dx = pos_x[i] - pos_x[i-1]
    #     dz = pos_z[i] - pos_z[i-1]
    #     distance = math.sqrt(dx**2 + dz**2)
    #     dT = time[i]-time[i-1]
    #     velocity = distance/ dT #velocity compared to previous datapoint
    #     if (pos_x[i] < n and pos_z[i] < m) and (pos_x[i] >= n-1 and pos_z[i] >= m-1):
    #         velo += velocity
    #         grid_count +=  1
    # if velo != 0:
    #     velo_AVG = (velo / grid_count)
    # else:
    #     velo_AVG = 0
    return  velo_AVG

def discritizer_vel(pos_x, pos_z, time):
    N = 8 #20   
    grid_count = np.zeros([N,N])
    velo = np.zeros([N,N])
    avg_speed = np.zeros([N,N])
    n_indx = 0

    pool = multiprocessing.Pool(processes=4)
    for n in range(-3,5):
        m_indx = 0
        print("index n",n_indx)
        if True:
            m_list = range(-4,4)
            # print("index m",m_indx)
            velo_AVG = list(  \
                pool.starmap(parallel_handler, \
                    zip(repeat(pos_x), repeat(pos_z), repeat(time), repeat(n), m_list)) )            
            # velo_AVG = list(  \
            #     starmap(parallel_handler, \
            #         zip(repeat(pos_x), repeat(pos_z), repeat(time), repeat(n), m_list)) )
            velo[n_indx,:] = velo_AVG
            # if velo_AVG != 0:
            #     grid_count[n_indx,m_indx] += 1
            
        n_indx = n_indx + 1
    avg_speed = np.transpose(velo)
    # for n_indx in range(0,N):
    #     for m_indx in range(0,N):
    #         if velo[n_indx,m_indx]  != 0  or  grid_count[n_indx, m_indx] != 0:
    #             avg_speed[n_indx,m_indx] = velo[n_indx,m_indx] / grid_count[n_indx, m_indx]
    #         else:
    #             avg_speed[n_indx,m_indx] = 0
    return avg_speed 

def main():
    #settings
    save_on = True
    plot_on = True
    overwrite_on = True
    #read data
    pos_x, pos_z, time = load_position("group_2.csv")
    
    # trajectory
    # if plot_on:
        # plot_traj(pos_x,pos_z)
    
    # Position
    # if os.path.exists('data_pos.npy') and not overwrite_on:
    #     print ("File exist")
    #     data = np.load("data_pos.npy")
    # else:
    #     print ("File does not exist, start calc")
    #     data = discritizer_pos(pos_x,pos_z)
    #     print("calc done")
    #     if save_on:
    #         print("save data")
    #         np.save("data_pos.npy", data)
    # if plot_on:
    #     plt.figure()
    #     plt.plot(pos_x+4,pos_z+4)
    #     # plt.grid()
    #     # plt.xlabel("position x [m]")
    #     # plt.ylabel("position z [m]")
    #     # plt.title("Trajectory")
    #     # plt.show()
    #     # plt.savefig("trajectory.png", dpi=300)
    #     x_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
    #     y_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]    
    #     # plt.figure()   
    #     ax = sns.heatmap(data, cmap="YlGnBu", xticklabels=x_axis_labels, yticklabels=y_axis_labels,  cbar_kws={'label': 'Frequency [%]'})
    #     ax.invert_yaxis()
    #     plt.title("Heatmap for position of the Bebop")
    #     plt.xlabel("x position")
    #     plt.ylabel("z position")
    #     plt.savefig("Heatmap_position.png", dpi=300)

    # Velocity
    
    if os.path.exists('data_vel.npy') and not overwrite_on:
        print ("File exist")
        data = np.load("data_vel.npy")
    else:
        print ("File does not exist, start calc")
        data = discritizer_vel(pos_x,pos_z,time)
        if save_on:
            print("save data")
            np.save("data_vel.npy", data)
    if plot_on:
        x_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
        y_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
        plt.figure()
        plt.plot(pos_x+4,pos_z+4)
        ax2 = sns.heatmap(data, cmap="YlGnBu",xticklabels=x_axis_labels, yticklabels=y_axis_labels, cbar_kws={'label': 'Speed [m/s]'})
        ax2.invert_yaxis()
        ax2.legend(["test"])
        plt.title("Heatmap for the average speed of the Bebop")
        plt.xlabel("x position")
        plt.ylabel("z position")
        plt.show()
        # plt.legend(["Average speed [m/s]"], loc ="lower right")
        plt.savefig("Heatmap_velocity.png", dpi=300)
    
if __name__ == "__main__":
    main()




