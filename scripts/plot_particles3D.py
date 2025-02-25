import matplotlib.pyplot as plt
from plot_domain import load_domain
from scipy.interpolate import griddata
import numpy as np
import os

COUNTER = 0

def load_particles(input_file, extension, counter):
    with open(f"{input_file}{counter}.{extension}", 'r') as f:
        lines = f.readlines()
        particles = []
        for line in lines:
            particle = line.strip().split()
            particles.append([float(value) for value in particle])

    return particles, counter

def plot_particles(particles, domain, output_dir, counter):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    fig, ax = plt.subplots()
    # Create grid data
    grid_x, grid_y = np.mgrid[min(domain[0]):max(domain[0]):100j, min(domain[1]):max(domain[1]):100j]
    grid_z = griddata((domain[0], domain[1]), domain[2], (grid_x, grid_y), method='cubic')

    contour = ax.contour(grid_x, grid_y, grid_z, 50, cmap='viridis', alpha=0.5)
    fig.colorbar(contour)
    scatter = ax.scatter([p[0] for p in particles], [p[1] for p in particles], color='black', marker='o', s=8)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_xlim(min(domain[0]), max(domain[0]))
    ax.set_ylim(min(domain[1]), max(domain[1]))


    plt.savefig(f"build/particles{counter}.png")



if __name__ == "__main__":
    domain = load_domain(input_file="build/domain.txt")
    while True:
        try:
            print("Plotting step", COUNTER)

            particles, COUNTER = load_particles(input_file="build/particles", extension="txt", counter=COUNTER)

            plot_particles(particles, domain, output_dir="build", counter=COUNTER)
            COUNTER += 1
        except FileNotFoundError:
            print(f"Step {COUNTER} not found")
            break