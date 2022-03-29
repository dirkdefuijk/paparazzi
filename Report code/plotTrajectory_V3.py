import numpy as np
import matplotlib.pylab as plt
import csv
import numpy as np; np.random.seed(0)
import seaborn as sns; sns.set_theme()
import scipy
import os.path

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
    
    for n in range(-3,5):
        m_indx = 0
        for m in range(-3,5):
            for i in range(len(pos_x)):
                if (pos_x[i] < n and pos_z[i] < m) and (pos_x[i] > n-1 and pos_z[i] > m-1):
                    grid_XY[n_indx,m_indx] = grid_XY[n,m] + 1
            m_indx = m_indx+1
        n_indx= n_indx+1

    return grid_XY            

def discritizer_vel(pos_x, pos_z, time):
    N = 8 #20   
    grid_count = np.zeros([N,N])
    velo = np.zeros([N,N])
    avg_speed = np.zeros([N,N])
    n_indx = 0
    velocity = 0 
    
    for n in range(-3,5):
        m_indx = 0
        print("index n",n_indx)
        for m in range(-3,5):
            print("index m",m_indx)
            for i in range(1, len(pos_x) ):
                A =[pos_x[i-1], pos_z[i-1]]
                B= [pos_x[i],pos_z[i]]
                pointss = np.array([ A, B ])
                distance = scipy.linalg.norm(pointss, 1)
                dT = time[i]-time[i-1]
                velocity = distance/ dT #velocity compared to previous datapoint
                if (pos_x[i] < n and pos_z[i] < m) and (pos_x[i] > n-1 and pos_z[i] > m-1):
                    velo[n_indx, m_indx] = velo[n_indx, m_indx] + velocity
                    grid_count[n_indx,m_indx] +=  1

            m_indx = m_indx + 1
        n_indx = n_indx + 1
    for n_indx in range(0,N):
        for m_indx in range(0,N):
            avg_speed[n_indx,m_indx] = velo[n_indx,m_indx] / grid_count[n_indx, m_indx]
    return avg_speed 

def main():
    #read data
    pos_x, pos_z, time = load_position("group_2.csv")
    
    # trajectory
    plot_traj(pos_x,pos_z)
    
    # Position
    if os.path.exists('data_pos.npy'):
        print ("File exist")
        data = np.load("data_pos.npy")
    else:
        print ("File does not exist, start calc")
        data = discritizer_pos(pos_x,pos_z)
        print("calc done, saving...")
        np.save("data_pos.npy", data)
    x_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
    y_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]    
    plt.figure()   
    ax = sns.heatmap(data, cmap="YlGnBu", xticklabels=x_axis_labels, yticklabels=y_axis_labels)
    ax.invert_yaxis()
    plt.title("Heatmap for position of the Bebop")
    plt.xlabel("x position")
    plt.ylabel("y position")
    plt.savefig("Heatmap_position.png", dpi=300)

    # Velocity
    
    if os.path.exists('data_vel.npy'):
        print ("File exist")
        data = np.load("data_vel.npy")
    else:
        print ("File does not exist, start calc")
        data = discritizer_vel(pos_x,pos_z,time)
        print("calc done, saving...")
        np.save("data_vel.npy", data)
   
    x_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
    y_axis_labels = [-4,-3,-2,-1,0,1,2,3,4]
    plt.figure()
    ax2 = sns.heatmap(data, cmap="YlGnBu",xticklabels=x_axis_labels, yticklabels=y_axis_labels)
    ax2.invert_yaxis()

    plt.title("Heatmap for velocity of the Bebop")
    plt.xlabel("x position")
    plt.ylabel("y position")
    plt.savefig("Heatmap_velocity.png", dpi=300)
    
if __name__ == "__main__":
    main()




