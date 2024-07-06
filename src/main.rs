use reqwest::blocking::Client;
use scraper::{Html, Selector};
use std::collections::HashSet;
use std::fs::{self, File};
use std::io::{BufRead, BufReader, Write};
use std::net::UdpSocket;
use std::path::Path;
use std::thread::sleep;
use std::time::Duration;

fn main() {
    // Create a client instance
    let client = Client::new();

    // Define the URL of the page to scrape
    let url = "https://www.njuskalo.hr/prodaja-kuca/samobor?price%5Bmax%5D=360000";

    // Send a GET request
    let response = client.get(url)
        .header("User-Agent", "Rust Scraper")
        .send()
        .expect("Failed to send request");

    // Ensure the request was successful
    if !response.status().is_success() {
        panic!("Failed to fetch page: {}", response.status());
    }

    // Get the response text
    let body = response.text().expect("Failed to read response text");

    // Parse the HTML
    let document = Html::parse_document(&body);

    // Define a selector to extract listing titles (adjust selector based on the website's structure)
    let selector = Selector::parse("article.entity-body h3.entity-title > a.link").expect("Failed to create selector");

    // Create a UDP socket
    let socket = UdpSocket::bind("0.0.0.0:0").expect("Couldn't bind to address");

    // Define the receiver address
    let receiver_addr = "192.168.0.11:4444";

    // Create a directory to save the downloaded text files
    let download_dir = "./downloads";
    fs::create_dir_all(download_dir).expect("Failed to create download directory");

    // Load the previously processed entries
    let processed_entries_path = "./processed_entries.log";
    let mut processed_entries = HashSet::new();
    if Path::new(processed_entries_path).exists() {
        let file = File::open(processed_entries_path).expect("Failed to open processed entries log file");
        let reader = BufReader::new(file);
        for line in reader.lines() {
            if let Ok(entry) = line {
                processed_entries.insert(entry);
            }
        }
    }

    // Track new entries
    let mut new_entries = Vec::new();

    // Iterate over the selected elements from the web page, save to text files, log to console, and send the text via UDP
    for (i, element) in document.select(&selector).enumerate() {
        if let Some(title) = element.text().next() {
            let message = title.trim();

            println!("Checking scraped entry: {}", message);  // Debug print

            if !processed_entries.contains(message) {
                processed_entries.insert(message.to_string());
                new_entries.push(message.to_string());

                // Save the heading to a text file
                let filename = format!("{}/ad_heading_{}.txt", download_dir, i);
                let mut file = File::create(&filename).expect("Failed to create file");
                writeln!(file, "{}", message).expect("Failed to write to file");

                // Log the filename to the console
                println!("Downloaded and saved: {}", filename);

                // Send the notification message via UDP
                let notification_message = format!("NEW_ENTRY: {}", filename);
                socket.send_to(notification_message.as_bytes(), receiver_addr).expect("Couldn't send data");

                // Add a delay of 1 second (1000 milliseconds)
                sleep(Duration::from_secs(1));
            }
        }
    }

    // Save the updated processed entries to the log file
    let mut file = File::create(processed_entries_path).expect("Failed to create processed entries log file");
    for entry in &processed_entries {
        writeln!(file, "{}", entry).expect("Failed to write to log file");
    }

    // Check if no elements were found
    if new_entries.is_empty() {
        println!("No new elements found matching the selector.");
    } else {
        println!("New entries found and sent via UDP.");
    }

    // Notify completion via UDP
    let completion_message = "Download and sending complete.";
    socket.send_to(completion_message.as_bytes(), receiver_addr).expect("Couldn't send data");
}