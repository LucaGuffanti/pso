import matplotlib.pyplot as plt
import os 
from scipy.interpolate import griddata
import numpy as np

def load_domain(input_file):
    if not os.path.exists(input_file):
        raise FileNotFoundError(f"File {input_file} not found")
    
    with open(input_file, 'r') as f:
        lines = f.readlines()
        physical_dimensions = int(lines[0].strip().split()[0])
        total_dimesions = physical_dimensions + 1

        data = [[] for _ in range(total_dimesions)]
        for line in lines[1:]:
            point_coord = line.strip().split()
            for i, value in enumerate(point_coord):
                data[i].append(float(value))
        
    return data

def plot_domain(data, output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
                
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    # Create grid data
    grid_x, grid_y = np.mgrid[min(data[0]):max(data[0]):100j, min(data[1]):max(data[1]):100j]
    grid_z = griddata((data[0], data[1]), data[2], (grid_x, grid_y), method='cubic')

    # Plot isosurface
    ax.contour3D(grid_x, grid_y, grid_z, 50, cmap='viridis')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    plt.savefig(f"{output_dir}/domain.png")
    plt.close()

if __name__ == "__main__":
    data = load_domain(input_file="build/domain.txt")
    plot_domain(data, output_dir="build")
