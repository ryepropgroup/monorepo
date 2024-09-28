import tkinter as tk
from tkinter import ttk
from time import strftime
from PIL import Image, ImageTk
import threading
import socket
import json
import ttkbootstrap as ttk
import time
 
# Constants
ORIGINAL_WINDOW_WIDTH = 1280
ORIGINAL_WINDOW_HEIGHT = 720
TCP_SERVER_PORT = 6000
PROGRESS_BAR_WIDTH = 20  # Adjust this value if the width of the progress bar changes

engine_computer_ip = None

NEW_WINDOW_WIDTH = 1920  # Enter new width
NEW_WINDOW_HEIGHT = 1040  # Enter new height

SCALE_X = NEW_WINDOW_WIDTH / ORIGINAL_WINDOW_WIDTH
SCALE_Y = NEW_WINDOW_HEIGHT / ORIGINAL_WINDOW_HEIGHT

# Function to scale positions
def scale_position(x, y, scale_x, scale_y):
    return int(x * scale_x), int(y * scale_y)

# Function to scale sizes
def scale_size(width, height, scale_x, scale_y):
    return int(width * scale_x), int(height * scale_y)

# Function to scale progress bars
def scale_progress_bar(x, y, orientation, length, scale_x, scale_y):
    scaled_x, scaled_y = scale_position(x, y, scale_x, scale_y)
    scaled_length = int(length * (scale_x if orientation == "horizontal" else scale_y))
    return scaled_x, scaled_y, orientation, scaled_length

# Example coordinates (to be adjusted based on visual inspection)
button_positions = {
    "v11_no": (185, 420),
    "v10": (297, 235),
    "v12_no": (301, 420),
    "v20": (945, 92),
    "v21": (487, 266),
    "v22": (784, 17),
    "v23_no": (607, 92),
    "v30": (945, 405),
    "v31": (487, 555),
    "v32": (486, 396),
    "v33": (707, 455),
    "v34": (558, 326),
    "v35_no": (484, 640),
    "v36": (629, 642),
    "v37": (727, 598),
    "v38_no": (819, 659),
}

state_positions = {
    "igniter": (1176, 632),
}

progress_bar_positions = {
    "p10": (147, 261, "horizontal", 2500),
    "p20": (1121, 161, "horizontal", 2500), #818 250
    "p30": (1141, 437, "horizontal", 2500), 
    "p21": (817, 250, "horizontal", 2500),
    "p31": (818, 399, "horizontal", 2500),
    "p22": (953, 230, "horizontal", 2500),
    "p32": (913, 337, "horizontal", 2500),
    "t2": (729, 227, "vertical", 1000),
    "t3": (635, 377, "vertical", 1000),
    "lc": (1159, 34, "horizontal", 1000)
}

sequences_start_position = (50, 600)
sequences_max_width = 300
sequences = ["reset", "ignition", "ematch_test", "oxidizer", "alternate_oxidizer", "cold_flow"]

# Scaled button positions
scaled_button_positions = {key: scale_position(x, y, SCALE_X, SCALE_Y) for key, (x, y) in button_positions.items()}
scaled_state_positions = {key: scale_position(x, y, SCALE_X, SCALE_Y) for key, (x, y) in state_positions.items()}

# Scaled progress bar positions
scaled_progress_bar_positions = {
    key: scale_progress_bar(x, y, orientation, length, SCALE_X, SCALE_Y)
    for key, (x, y, orientation, length) in progress_bar_positions.items()
}

class GUIApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Valve Control and Sensor Monitoring")
        self.root.geometry(f"{NEW_WINDOW_WIDTH}x{NEW_WINDOW_HEIGHT}")
        self.root.resizable(False, False)  # Disable window resizing

        # Load and display background image
        self.background_image = Image.open("pid_v4.png")
        self.background_image = self.background_image.resize(
            (NEW_WINDOW_WIDTH, NEW_WINDOW_HEIGHT), Image.LANCZOS
        )
        self.bg_label = tk.Label(self.root)
        self.bg_label.place(x=0, y=0, width=NEW_WINDOW_WIDTH, height=NEW_WINDOW_HEIGHT)
        self.update_background_image()

        # Create the clock label
        self.clock_label = ttk.Label(self.root, font=("Helvetica", 15), background="#FFFFFF", foreground="red")
        self.clock_label.place(x=100, y=200)
        self.update_time()

        # Create buttons and progress bars
        self.create_widgets()

        self.start_listening_thread(6969)
        # global engine_computer_ip
        # engine_computer_ip = "127.0.0.1"
        # self.start_tcp_communication()

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
        # Style for labels
        style.configure(
            "TLabel",
            font=("Helvetica", 12),
            background="white",
            foreground="black",
            anchor="center",
            justify="center",
        )
        
        # Place data collection label
        (x, y) = scale_size(250, 50 - 25, SCALE_X, SCALE_Y)
        ttk.Label(self.root, text="Data Collection", style="TLabel").place(x=x, y=y)
        
        # Create data start and end buttons
        scaled_x, scaled_y = scale_position(250, 50, SCALE_X, SCALE_Y)
        scaled_width, scaled_height = scale_size(100, 25, SCALE_X, SCALE_Y)
        start_data_btn = ttk.Button(
            self.root,
            text="Start Recording",
            style="Open.TButton",
            command=lambda: self.send_message("go:start_data\n"),
        )
        start_data_btn.place(x=scaled_x, y=scaled_y, width=scaled_width, height=scaled_height)
        
        scaled_x, scaled_y = scale_position(250, 85, SCALE_X, SCALE_Y)
        scaled_width, scaled_height = scale_size(100, 25, SCALE_X, SCALE_Y)
        start_data_btn = ttk.Button(
            self.root,
            text="Stop Recording",
            style="Close.TButton",
            command=lambda: self.send_message("go:stop_data\n"),
        )
        start_data_btn.place(x=scaled_x, y=scaled_y, width=scaled_width, height=scaled_height)

        # Creating buttons
        self.valve_buttons = {}
        for valve, (x, y) in scaled_button_positions.items():
            scaled_width, scaled_height = scale_size(72, 25, SCALE_X, SCALE_Y)
            open_btn = ttk.Button(
                self.root,
                text="Open",
                style="Open.TButton",
                command=lambda v=valve: self.control_valve(v, True),
            )
            open_btn.place(x=x, y=y, width=scaled_width, height=scaled_height)

            close_btn = ttk.Button(
                self.root,
                text="Close",
                style="Close.TButton",
                command=lambda v=valve: self.control_valve(v, False),
            )
            close_btn.place(
                x=x, y=y + scaled_height, width=scaled_width, height=scaled_height
            )  # Adjusted to be vertically touching

            self.valve_buttons[valve] = (open_btn, close_btn)

        # Creating state buttons (no toggle)
        self.state_buttons = {}
        for name, (x, y) in scaled_state_positions.items():
            scaled_width, scaled_height = scale_size(72, 25, SCALE_X, SCALE_Y)
            btn = ttk.Button(
                self.root,
                text="LOW",
                style="Close.TButton",
            )
            
            btn.place(x=x, y=y, width=scaled_width, height=scaled_height)

            self.state_buttons[name] = (btn)
        
        # Place sequence label
        (x, y) = scale_size(sequences_start_position[0], sequences_start_position[1] - 25, SCALE_X, SCALE_Y)
        ttk.Label(self.root, text="Sequences", style="TLabel").place(x=x, y=y)

        # Place sequence buttons
        self.sequence_buttons = {}
        scaled_seq_width, scaled_seq_height = scale_size(70, 25, SCALE_X, SCALE_Y)
        scaled_max_width = int(sequences_max_width * SCALE_X)
        (x_start, y_start) = scale_size(sequences_start_position[0], sequences_start_position[1], SCALE_X, SCALE_Y)
        (x, y) = (x_start, y_start)
        spacing = int(12 * SCALE_X)
        for button in sequences:
            btn = ttk.Button(
                self.root,
                text=button,
                style="Sequence.TButton",
                command=lambda v=button: self.send_message(f"sequence:{v}\n"),
            )
            btn.place(x=x, y=y, width=scaled_seq_width, height=scaled_seq_height)
            self.sequence_buttons[button] = btn
            x += scaled_seq_width + spacing
            if x >= scaled_max_width:
                x = x_start
                y += scaled_seq_height + spacing

        # Creating progress bars
        self.sensor_bars = {}
        self.sensor_labels = {}
        for sensor, (x, y, orientation, maxval) in scaled_progress_bar_positions.items():
            if orientation == "vertical":
                bar = ttk.Progressbar(
                    self.root, orient="vertical", mode="determinate", maximum=maxval
                )
                bar.place(x=x, y=y, height=int(88 * SCALE_Y), width=int(PROGRESS_BAR_WIDTH * SCALE_X))
                label = ttk.Label(
                    self.root,
                    text="0.00",
                    font=("Helvetica", 10),
                    background="#333",
                    foreground="white",
                )
                label.place(x=x + int(PROGRESS_BAR_WIDTH * SCALE_X) // 2, y=y + int(90 * SCALE_Y), anchor="center")
            else:
                bar = ttk.Progressbar(
                    self.root, orient="horizontal", mode="determinate", maximum=maxval
                )
                bar.place(x=x, y=y, width=int(95 * SCALE_X), height=int(PROGRESS_BAR_WIDTH * SCALE_Y))
                label = ttk.Label(
                    self.root,
                    text="0.00",
                    font=("Helvetica", 10),
                    background="#333",
                    foreground="white",
                )
                label.place(x=x + int(50 * SCALE_X), y=y + int(PROGRESS_BAR_WIDTH * SCALE_Y) + int(10 * SCALE_Y), anchor="center")

            self.sensor_bars[sensor] = bar
            self.sensor_labels[sensor] = label

    def update_time(self):
        current_time = strftime('%H:%M:%S %p')
        self.clock_label.config(text=current_time)
        self.clock_label.after(1000, self.update_time)

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
                print(f"MACH: Sending message: {message.strip()}")
                self.sock.sendall(message.encode('utf-8'))
            except ConnectionError as e:
                print(f"MACH: Connection error: {e}")

    def _start_tcp_communication(self):
        global engine_computer_ip
        if not engine_computer_ip:
            return
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            print(f"MACH: Connecting to server at {engine_computer_ip}:{TCP_SERVER_PORT}.")
            self.sock.connect((engine_computer_ip, TCP_SERVER_PORT))
            thread = threading.Thread(target=self.tcp_communication_thread, daemon=True)
            thread.start()
            thread.join()
            
            print(f"MACH: Lost connection to server, waiting for next broadcast.")
            
        except ConnectionError as e:
            print(f"MACH: Failed to connect to server: {e}, waiting for next broadcast.")
            
        finally:
            self.sock.close()
            self.sock = None
            engine_computer_ip = None
                

    def start_tcp_communication(self):
        threading.Thread(target=self._start_tcp_communication, daemon=True).start()

    def tcp_communication_thread(self):
        while True:
            try:
                data = self.sock.recv(1024)
                if data:
                    self.update_gui(data.decode("utf-8"))
            except ConnectionError as e:
                print(f"MACH: Connection error: {e}, disconnecting!")
                break

    def update_gui(self, data):
        try:
            data = json.loads(data)
        except Exception as e:
            return
            
        values = data.get("values", {})
        for valve, state in values.items():
            print(valve, state)
            valve = valve.lower()
            state = state == 1
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
            state_btn = self.state_buttons.get(valve)
            if state_btn:
                if state:
                    state_btn.config(text="HIGH", style="Open.TButton")
                else:
                    state_btn.config(text="LOW", style="Close.TButton")

        for sensor, value in values.items():
            sensor = sensor.lower()
            bar = self.sensor_bars.get(sensor)
            label = self.sensor_labels.get(sensor)
            if bar and label:
                bar["value"] = value
                label.config(text=f"{value:.2f}")
        # self.root.after(1000, self.update_gui_periodically)

    def update_gui_periodically(self):
        pass  # Implementation can include periodic tasks if needed
    
    def update_server_address(self, ip):
        global engine_computer_ip
        if engine_computer_ip:
            return
        
        print(f"MACH: Found MACH Engine Computer at IP: {ip}.")
        engine_computer_ip = ip
        # Start TCP communication thread
        self.start_tcp_communication()


    def listen_for_broadcast(self, port):
        sock = None
        
        while True:
            # Receive broadcast.
            try:
                if not sock:
                    # Create a UDP socket
                    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    # Enable the socket to receive broadcasts.
                    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
                    sock.bind(("0.0.0.0", port))
                    print(f"MACH: Listening for server broadcast messages on port {port}.")  
                data, addr = sock.recvfrom(1024)  # Buffer size is 1024 bytes
                message = data.decode('utf-8')
                if message == "MACH Engine Computer":
                    self.update_server_address(addr[0])
            except ConnectionError as e:
                print(f"MACH: Broadcast connection error: {e}, retrying in 5 seconds.")
                sock.close()
                sock = None
                time.sleep(5)
                
            except Exception as _:
                continue

    def start_listening_thread(self, port):
        listener_thread = threading.Thread(target=self.listen_for_broadcast, args=(port,))
        # Makes the thread exit when the main program exits.
        listener_thread.daemon = True
        listener_thread.start()
        return listener_thread

if __name__ == "__main__":
    root = ttk.Window(themename="darkly")
    app = GUIApp(root)
    root.mainloop()
