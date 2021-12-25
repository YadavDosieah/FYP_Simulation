#!/bin/bash

cd "$(dirname "$0")"

./Analysis.sh 0 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-C.A.png"

./Analysis.sh 1 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-C.B.png"

./Analysis.sh 2 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-C.C.png"

./Analysis.sh 3 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-SC.C.png"

./Analysis.sh 4 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-SC.A.png"

./Analysis.sh 5 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-SC.B.png"

./Analysis.sh 6 10 15 15 && ./plot_velocity.py && mv "Velocity Plot.png" "Velocity Plot-Heterogenous.png"
