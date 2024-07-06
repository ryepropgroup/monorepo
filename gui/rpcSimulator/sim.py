import grpc
import service_pb2
import service_pb2_grpc
import messages_pb2
import time
import random


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


def run():
    # Create a channel to the server
    channel = grpc.insecure_channel("localhost:50051")

    # Create a stub (client)
    stub = service_pb2_grpc.ServiceStub(channel)

    # Create a generator for sending sensor data
    def sensor_data_generator():
        while True:  # Send 10 random data points
            json_data = generate_random_values()
            data = messages_pb2.SensorData(
                timestamp=int(time.time()),
                values={
                    "p1_val": json_data["lj"]["p10val"],
                    "p2_val": json_data["lj"]["p21val"],
                    "p3_val": json_data["lj"]["p31val"],
                    "p20_val": json_data["lj"]["p20val"],
                    "p30_val": json_data["lj"]["p30val"],
                    "p22_val": json_data["lj"]["p22val"],
                    "p32_val": json_data["lj"]["p32val"],
                    "t2_thermo": json_data["lj"]["t1val"],
                    "t3_thermo": json_data["lj"]["t2val"],
                    "pinJval": json_data["lj"]["pinjval"],
                    "lcval": json_data["lj"]["lcval"],
                    "inj1val": json_data["lj"]["inj1val"],
                    "inj2val": json_data["lj"]["inj2val"],
                    "ignVal": json_data["lj"]["ignval"],
                    "v11_s": json_data["valves"]["V11_NO"],
                    "v10_sb": json_data["valves"]["V10"],
                    "v12_s": json_data["valves"]["V12_NO"],
                    "v20_sb": json_data["valves"]["V20"],
                    "v21_sb": json_data["valves"]["V21"],
                    "v22_sb": json_data["valves"]["V22"],
                    "v23_sb": json_data["valves"]["V23_NO"],
                    "v30_sb": json_data["valves"]["V30"],
                    "v31_sb": json_data["valves"]["V31"],
                    "v32_s": json_data["valves"]["V32"],
                    "v33_sb": json_data["valves"]["V33"],
                    "v34_s": json_data["valves"]["V34"],
                    "v35_sb": json_data["valves"]["V35_NO"],
                    "v36_s": json_data["valves"]["V36"],
                    "v37_sb": json_data["valves"]["V37"],
                    "v38_s": json_data["valves"]["V38_NO"],
                    "v_ign": json_data["valves"]["IGN"],
                    "fileWriteState": json_data["writing"],
                },
            )
            yield data
            time.sleep(0.5)

    # Send the sensor data to the server and get the response
    response = stub.SensorDataStream(sensor_data_generator())

    # Print the response from the server
    print("Response from server:", response)


if __name__ == "__main__":
    run()
