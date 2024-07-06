import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import threading
import socket
import json
import ttkbootstrap as ttk

# Constants
WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720
TCP_SERVER_IP = "127.0.0.1"
TCP_SERVER_PORT = 6000
PROGRESS_BAR_WIDTH = 20  # Adjust this value if the width of the progress bar changes

# Example coordinates (to be adjusted based on visual inspection)
button_positions = {
    "v11_s": (185, 420),
    "v10_sb": (297, 235),
    "v12_s": (300, 420),
    "v20_sb": (946, 95),
    "v21_sb": (487, 266),
    "v22_sb": (784, 17),
    "v23_sb": (600, 92),
    "v30_sb": (945, 405),
    "v31_sb": (488, 558),
    "v32_s": (486, 396),
    "v33_sb": (707, 455),
    "v34_s": (558, 326),
    "v35_sb": (484, 644),
    "v36_s": (629, 642),
    "v37_sb": (729, 598),
    "v38_s": (820, 659),
}

progress_bar_positions = {
    "p1_val": (146, 261, "horizontal"),
    "p2_val": (818, 250, "horizontal"),
    "p3_val": (817, 399, "vertical"),
    "p20_val": (1122, 161, "horizontal"),
    "p30_val": (1142, 437, "horizontal"),
    "p22_val": (954, 230, "horizontal"),
    "p32_val": (914, 337, "horizontal"),
    "t2_thermo": (734, 230, "vertical"),
    "t3_thermo": (639, 378, "vertical"),
    # "pinJval": (300, 500, "horizontal"),
    # "lcval": (300, 500, "horizontal"),
}

class GUIApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Valve Control and Sensor Monitoring")
        self.root.geometry(f"{WINDOW_WIDTH}x{WINDOW_HEIGHT}")
        self.root.resizable(False, False)  # Disable window resizing

        # Load and display background image
        self.background_image = Image.open("pid.png")
        self.background_image = self.background_image.resize(
            (WINDOW_WIDTH, WINDOW_HEIGHT), Image.LANCZOS
        )
        self.bg_label = tk.Label(self.root)
        self.bg_label.place(x=0, y=0, width=WINDOW_WIDTH, height=WINDOW_HEIGHT)
        self.update_background_image()

        # Create buttons and progress bars
        self.create_widgets()

        # Start TCP communication thread
        self.start_tcp_communication()

    def update_background_image(self):
        # Resize background image to fit the static window size
        self.bg_image = ImageTk.PhotoImage(self.background_image)
        self.bg_label.config(image=self.bg_image)

    def create_widgets(self):
        style = ttk.Style()

        # Custom style for open button
        style.configure(
            "Open.TButton",
            relief="flat",
            borderwidth=1,
            padding=5,
            font=("Helvetica", 10),
            focuscolor="none",
            background="#4CAF50",
            foreground="white",
            anchor="center",
            justify="center",
        )

        # Custom style for close button
        style.configure(
            "Close.TButton",
            relief="flat",
            borderwidth=1,
            padding=5,
            font=("Helvetica", 10),
            focuscolor="none",
            background="#f44336",
            foreground="white",
            anchor="center",
            justify="center",
        )

        # Creating buttons
        self.valve_buttons = {}
        for valve, (x, y) in button_positions.items():
            open_btn = ttk.Button(
                self.root,
                text="Open",
                style="Open.TButton",
                command=lambda v=valve: self.control_valve(v, True),
            )
            open_btn.place(x=x, y=y, width=72, height=25)

            close_btn = ttk.Button(
                self.root,
                text="Close",
                style="Close.TButton",
                command=lambda v=valve: self.control_valve(v, False),
            )
            close_btn.place(
                x=x, y=y + 25, width=72, height=25
            )  # Adjusted to be vertically touching

            self.valve_buttons[valve] = (open_btn, close_btn)

        # Creating progress bars
        self.sensor_bars = {}
        self.sensor_labels = {}
        for sensor, (x, y, orientation) in progress_bar_positions.items():
            if orientation == "vertical":
                bar = ttk.Progressbar(
                    self.root, orient="vertical", mode="determinate", maximum=100
                )
                bar.place(x=x, y=y, height=100, width=PROGRESS_BAR_WIDTH)
                label = ttk.Label(
                    self.root,
                    text="0.00",
                    font=("Helvetica", 10),
                    background="#333",
                    foreground="white",
                )
                label.place(x=x + PROGRESS_BAR_WIDTH // 2, y=y + 110, anchor="center")
            else:
                bar = ttk.Progressbar(
                    self.root, orient="horizontal", mode="determinate", maximum=100
                )
                bar.place(x=x, y=y, width=95, height=PROGRESS_BAR_WIDTH)
                label = ttk.Label(
                    self.root,
                    text="0.00",
                    font=("Helvetica", 10),
                    background="#333",
                    foreground="white",
                )
                label.place(x=x + 50, y=y + PROGRESS_BAR_WIDTH + 10, anchor="center")

            self.sensor_bars[sensor] = bar
            self.sensor_labels[sensor] = label

    def control_valve(self, valve, state):
        # Send valve control command to server
        pass  # Implementation depends on server protocol

    def start_tcp_communication(self):
        threading.Thread(target=self.tcp_communication_thread, daemon=True).start()

    def tcp_communication_thread(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((TCP_SERVER_IP, TCP_SERVER_PORT))
            while True:
                data = sock.recv(1024)
                if data:
                    self.update_gui(data.decode('utf-8'))

    def update_gui(self, data):
        data = json.loads(data)
        values = data.get("values", {})

        for valve, state in values.items():
            if valve.startswith("v"):
                open_btn, close_btn = self.valve_buttons.get(valve, (None, None))
                if open_btn and close_btn:
                    if state:
                        open_btn.config(state="disabled", bootstyle="secondary")
                        close_btn.config(state="normal", bootstyle="danger")
                    else:
                        open_btn.config(state="normal", bootstyle="success")
                        close_btn.config(state="disabled", bootstyle="secondary")

        for sensor, value in values.items():
            if sensor.startswith("p"):
                bar = self.sensor_bars.get(sensor)
                label = self.sensor_labels.get(sensor)
                if bar and label:
                    bar["value"] = value
                    label.config(text=f"{value:.2f}")

        # self.root.after(1000, self.update_gui_periodically)

    def update_gui_periodically(self):
        pass  # Implementation can include periodic tasks if needed


if __name__ == "__main__":
    root = ttk.Window(themename="darkly")
    app = GUIApp(root)
    root.mainloop()
