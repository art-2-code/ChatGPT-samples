use rand::Rng;
use serde_json::json;
use std::env;
use std::fs::File;
use std::io::Write;
use std::process::exit;

fn main() {
    // -----------------------------
    // Parse command-line arguments
    // -----------------------------
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("Usage: {} <ArraySize> <MaxRange>", args[0]);
        exit(1);
    }

    let array_size: usize = args[1].parse().expect("ArraySize must be an integer");
    let max_range: f64 = args[2].parse().expect("MaxRange must be a number");

    if array_size == 0 {
        eprintln!("ArraySize must be greater than 0");
        exit(1);
    }
    if max_range <= 0.0 {
        eprintln!("MaxRange must be greater than 0");
        exit(1);
    }

    // -----------------------------
    // Generate random matrix
    // -----------------------------
    let mut rng = rand::thread_rng();

    let mut matrix: Vec<Vec<String>> = Vec::new();

    for _ in 0..array_size {
        let mut row: Vec<String> = Vec::new();
        for _ in 0..array_size {
            let val: f64 = rng.gen_range(0.0..max_range);
            row.push(format!("{:.4e}", val)); // #.####e# format
        }
        matrix.push(row);
    }

    // -----------------------------
    // Convert to JSON
    // -----------------------------
    let json_matrix = json!(matrix);

    // -----------------------------
    // Write to file
    // -----------------------------
    let mut file = File::create("matrix.json").expect("Failed to create output file");
    let json_string = serde_json::to_string_pretty(&json_matrix)
        .expect("Failed to serialize JSON");

    file.write_all(json_string.as_bytes())
        .expect("Failed to write JSON file");

    println!("Matrix written to matrix.json");
}