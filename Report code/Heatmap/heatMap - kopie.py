import numpy as np
import matplotlib.pylab as plt
import csv
import seaborn as sb

def load_position(filename):
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        pos_x = np.zeros((215591)) # no. datapoints
        pos_z = np.zeros((215591))
        for row in reader:
            pos_x[int(row["Frame"])] = float(row['pos_X'])
            pos_z[int(row["Frame"])] = float(row['pos_Z'])
    return pos_x, pos_z

def plot_traj(pos_x, pos_z):
    plt.figure()
    plt.plot(pos_x,pos_z)
    plt.grid()
    plt.xlabel("position x [m]")
    plt.ylabel("position z [m]")
    plt.show()


def heatmap(pos_x, pos_z):
    axes = np.zeros((8,8))
    for i, x_l in enumerate(np.arange(-3, 5)):
        x_u = x_l + 1

        for j, z_l in enumerate(np.arange(-3, 5)):
            z_u = z_l + 1

            hits = np.multiply(pos_x >= x_l, pos_x < x_u)
            hits = np.multiply(pos_z >= z_l, hits)
            hits = np.multiply(pos_z  < z_u, hits)

            axes[i,j] = sum(hits)

    ax = sb.heatmap(np.transpose(axes))
    ax.invert_yaxis()
    ax.plot(pos_x+3,pos_z+3)
    plt.show()


def main():
    pos_x, pos_z = load_position("group_2.csv")
    heatmap(pos_x,pos_z)
    



if __name__ == "__main__":
    main()




