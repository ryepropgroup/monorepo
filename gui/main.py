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
    "v11_no": (185, 420),
    "v10": (297, 235),
    "v12_no": (300, 420),
    "v20": (946, 95),
    "v21": (487, 266),
    "v22": (784, 17),
    "v23_no": (600, 92),
    "v30": (945, 405),
    "v31": (488, 558),
    "v32": (486, 396),
    "v33": (707, 455),
    "v34": (558, 326),
    "v35_no": (484, 644),
    "v36": (629, 642),
    "v37": (729, 598),
    "v38_no": (820, 659),
}

progress_bar_positions = {
    "p10": (146, 261, "horizontal", 2500),
    "p20": (818, 250, "horizontal", 2500),
    "p30": (817, 399, "horizontal", 2500),
    "p20": (1122, 161, "horizontal", 2500),
    "p30": (1142, 437, "horizontal", 2500),
    "p22": (954, 230, "horizontal", 2500),
    "p32": (914, 337, "horizontal", 2500),
    "t2": (729, 227, "vertical", 1000),
    "t3": (639, 378, "vertical", 1000),
    "pinj": (1155, 654, "horizontal", 1000),
    "p21": (817, 250, "horizontal", 2500),
    "p31": (819, 398, "horizontal", 2500),
    # "pinJ": (300, 500, "horizontal"),
    # "lc": (300, 500, "horizontal"),
}

sequence_positions ={
    "stop": (65,600),
    "ignition": (135,600),
    "igniter": (205,600),
    "oxidizer": (275,600),
    "alternate_oxidizer": (345,600),
    "cold_flow": (415,600),
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

        style.configure(
            "Sequence.TButton",
            relief="flat",
            borderwidth=1,
            padding=5,
            font=("Helvetica", 10),
            focuscolor="none",
            background="#FF5733",
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

        self.sequence_buttons = {}

        for button, (x, y) in sequence_positions.items():
            button = ttk.Button(
                self.root,
                text=button,
                style="Sequence.TButton",
                command=lambda v=button: self.send_message(f"sequence:{v}\n"),
            )
            button.place(x=x, y=y, width=70, height=25)
            self.sequence_buttons[button] = button

        # Creating progress bars
        self.sensor_bars = {}
        self.sensor_labels = {}
        for sensor, (x, y, orientation, maxval) in progress_bar_positions.items():
            if orientation == "vertical":
                bar = ttk.Progressbar(
                    self.root, orient="vertical", mode="determinate", maximum=maxval
                )
                bar.place(x=x, y=y, height=88, width=PROGRESS_BAR_WIDTH)
                label = ttk.Label(
                    self.root,
                    text="0.00",
                    font=("Helvetica", 10),
                    background="#333",
                    foreground="white",
                )
                label.place(x=x + PROGRESS_BAR_WIDTH // 2, y=y + 90, anchor="center")
            else:
                bar = ttk.Progressbar(
                    self.root, orient="horizontal", mode="determinate", maximum=maxval
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
        if state == True:
            state = "open"
        else:
            state = "close"
        message = f"valve:{valve}:{state}\n"
        self.send_message(message)

    def send_message(self, message):
        if hasattr(self, 'sock') and self.sock:
            try:
                self.sock.sendall(message.encode('utf-8'))
            except ConnectionError as e:
                print(f"Connection error: {e}")

    def start_tcp_communication(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect((TCP_SERVER_IP, TCP_SERVER_PORT))
            threading.Thread(target=self.tcp_communication_thread, daemon=True).start()
        except ConnectionError as e:
            print(f"Failed to connect to server: {e}")

    def tcp_communication_thread(self):
        while True:
            try:
                data = self.sock.recv(1024)
                if data:
                    self.update_gui(data.decode("utf-8"))
            except ConnectionError as e:
                print(f"Connection error: {e}")
                break

    def update_gui(self, data):
        try:
            data = json.loads(data)
        except Exception as e:
            # print(e)
            # print(data)
            return
            
        values = data.get("values", {})
        for valve, state in values.items():
            valve = valve.lower()
            state = state == 1
            if valve.startswith("v"):
                open_btn, close_btn = self.valve_buttons.get(valve, (None, None))
                if open_btn and close_btn:
                    if not "_no" in valve:
                        state = not state
                    if state:
                        open_btn.config(state="disabled")
                        close_btn.config(state="normal")
                    else:
                        open_btn.config(state="normal")
                        close_btn.config(state="disabled")

        for sensor, value in values.items():
            sensor = sensor.lower()
            if sensor.startswith("p") or sensor.startswith("t"):
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
