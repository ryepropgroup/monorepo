import grpc
import service_pb2
import service_pb2_grpc
import messages_pb2
import time
import random
import threading


def generate_random_values():
    return {
        "lj": {
            "p10val": random.uniform(0.0, 2500.0),
            "p21val": random.uniform(0.0, 2500.0),
            "p31val": random.uniform(0.0, 2500.0),
            "p20val": random.uniform(0.0, 2500.0),
            "p30val": random.uniform(0.0, 2500.0),
            "p22val": random.uniform(0.0, 2500.0),
            "p32val": random.uniform(0.0, 2500.0),
            "t1val": random.uniform(0, 1000.0),
            "t2val": random.uniform(0, 1000.0),
            "pinjval": random.uniform(0.0, 2500.0),
            "lcval": random.uniform(0.0, 1000.0),
            "inj1val": random.uniform(0.0, 2500.0),
            "inj2val": random.uniform(0.0, 2500.0),
            "ignval": random.uniform(0.0, 1000.0),
        },
        "valves": {
            "V11_NO": random.choice([True, False]),
            "V10": random.choice([True, False]),
            "V12_NO": random.choice([True, False]),
            "V20": random.choice([True, False]),
            "V21": random.choice([True, False]),
            "V22": random.choice([True, False]),
            "V23_NO": random.choice([True, False]),
            "V30": random.choice([True, False]),
            "V31": random.choice([True, False]),
            "V32": random.choice([True, False]),
            "V33": random.choice([True, False]),
            "V34": random.choice([True, False]),
            "V35_NO": random.choice([True, False]),
            "V36": random.choice([True, False]),
            "V37": random.choice([True, False]),
            "V38_NO": random.choice([True, False]),
            "IGN": random.choice([True, False]),
        },
        "writing": random.choice([True, False]),
    }


def run_sensor_data_stream(stub):
    # Create a generator for sending sensor data
    def sensor_data_generator():
        while True:
            json_data = generate_random_values()
            data = messages_pb2.SensorData(
                timestamp=int(time.time()),
                values={
                    "p1": json_data["lj"]["p10val"],
                    "p2": json_data["lj"]["p21val"],
                    "p3": json_data["lj"]["p31val"],
                    "p20": json_data["lj"]["p20val"],
                    "p30": json_data["lj"]["p30val"],
                    "p22": json_data["lj"]["p22val"],
                    "p32": json_data["lj"]["p32val"],
                    "t2": json_data["lj"]["t1val"],
                    "t3": json_data["lj"]["t2val"],
                    "pinJ": json_data["lj"]["pinjval"],
                    "lc": json_data["lj"]["lcval"],
                    "inj1": json_data["lj"]["inj1val"],
                    "inj2": json_data["lj"]["inj2val"],
                    "ign": json_data["lj"]["ignval"],
                    "v11_no": json_data["valves"]["V11_NO"],
                    "v10": json_data["valves"]["V10"],
                    "v12_no": json_data["valves"]["V12_NO"],
                    "v20": json_data["valves"]["V20"],
                    "v21": json_data["valves"]["V21"],
                    "v22": json_data["valves"]["V22"],
                    "v23": json_data["valves"]["V23_NO"],
                    "v30": json_data["valves"]["V30"],
                    "v31": json_data["valves"]["V31"],
                    "v32": json_data["valves"]["V32"],
                    "v33": json_data["valves"]["V33"],
                    "v34": json_data["valves"]["V34"],
                    "v35_no": json_data["valves"]["V35_NO"],
                    "v36": json_data["valves"]["V36"],
                    "v37": json_data["valves"]["V37"],
                    "v38_no": json_data["valves"]["V38_NO"],
                    "igniter": json_data["valves"]["IGN"],
                    "fileWriteState": json_data["writing"],
                },
            )
            yield data
            time.sleep(0.5)

    # Send the sensor data to the server
    stub.SensorDataStream(sensor_data_generator())


def run_command_stream(stub):
    device_info = messages_pb2.DeviceInformation()  # Empty DeviceInformation

    # Send the DeviceInformation message
    command_stream = stub.CommandStream(device_info)

    # Process the commands received from the server
    for command in command_stream:
        print("Received command from server:", command)


def run():
    # Create a channel to the server
    channel = grpc.insecure_channel("localhost:50051")

    # Create a stub (client)
    stub = service_pb2_grpc.EngineComputerStub(channel)

    # Create threads for sensor data stream and command stream
    sensor_thread = threading.Thread(target=run_sensor_data_stream, args=(stub,))
    command_thread = threading.Thread(target=run_command_stream, args=(stub,))

    # Start the threads
    sensor_thread.start()
    command_thread.start()

    # Wait for threads to complete (in this case, they won't, but join them anyway)
    sensor_thread.join()
    command_thread.join()


if __name__ == "__main__":
    run()
