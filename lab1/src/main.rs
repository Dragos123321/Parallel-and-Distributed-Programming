use std::{sync::{Mutex, Arc}, io::{BufReader, BufRead, Write}, time::Duration};
use std::fs::File;

use rand::Rng;

use crate::account::Account;

pub mod account;

fn main() {
    let current_transaction_number: Arc<Mutex<u32>> = Arc::new(Mutex::new(0));

    let mut is_finished: bool = false;

    let file = File::open("./accounts_data.txt").expect("No file found.");
    let reader = BufReader::new(file);
   
    let mut accounts: Vec<Arc<Mutex<Account>>> = Vec::new();

    for (_, line) in reader.lines().enumerate() {
        let line = String::from(line.unwrap().trim());

        let vec_line = line.split(";").collect::<Vec<&str>>();

        accounts.push(Arc::new(Mutex::new(Account::new(String::from(vec_line[0]),
                                 String::from(vec_line[1]).parse::<u64>().unwrap(), 
                             String::from(vec_line[2]).parse::<u64>().unwrap(), 
                                String::from(vec_line[3]),
                        String::from(vec_line[2]).parse::<u64>().unwrap()))));
    }

    let threads_per_run = 10;
    let accounts_len = accounts.len();

    while !is_finished {
        print!("Enter command: ");
        std::io::stdout().flush().unwrap();

        let mut command = String::from("");
        std::io::stdin().read_line(&mut command).expect("Invalid command.");

        command = String::from(command.trim());
       
        if command == "start" {
            println!("\n--------------------------------------------------\n");
    
            let mut threads = vec![];

            for index in 0..threads_per_run {
                let mut index_from = generate_random_index(0, accounts_len);
                let mut index_to = generate_random_index(0, accounts_len);

                while index_from == index_to {
                    index_from = generate_random_index(0, accounts_len);
                    index_to = generate_random_index(0, accounts_len);
                }

                let index_from = index_from;
                let index_to = index_to;
                
                let account_from_ref = Arc::clone(&accounts[index_from]);
                let account_to_ref = Arc::clone(&accounts[index_to]);
                let current_transaction_number_ref = Arc::clone(&current_transaction_number);

                threads.push(std::thread::spawn(move || {
                    let mut amount = generate_amount();
                    let mut account_from_guard = account_from_ref.lock().unwrap();
                    let mut account_to_guard = account_to_ref.lock().unwrap();

                    while amount > account_from_guard.amount {
                        amount = generate_amount();
                    }

                    let mut transaction_number = current_transaction_number_ref.lock().unwrap();

                    let log = transaction_number.to_string() + ":" + &account_from_guard.id.to_string() + "->" + &amount.to_string() + "->" + &account_to_guard.id.to_string();
                    *transaction_number += 1;

                    account_from_guard.amount -= amount;
                    account_to_guard.amount += amount;

                    if account_from_guard.log == "" {
                        account_from_guard.log += &log;
                    } 
                    else { 
                        account_from_guard.log += &(String::from(",") + log.as_str());
                    }
                    
                    if account_to_guard.log == "" {
                        account_to_guard.log += &log;
                    } 
                    else { 
                        account_to_guard.log += &(String::from(",") + log.as_str());
                    }

                    println!("{} from {} to {} with log: {}", amount, account_from_guard.iban, account_to_guard.iban, log);
                }));
                
                if index % 3 == 0 {
                    std::thread::sleep(Duration::from_millis(1));
                    for account in &accounts {
                        let account_ref = Arc::clone(&account);
                        let account_guard = account_ref.lock().unwrap();

                        verify(&account_guard, account_guard.init_amount, account_guard.amount);
                    }
                }
            }

            for thread in threads {
                thread.join().unwrap();
            }

            for account in &accounts {
                let account_ref = Arc::clone(&account);
                let account_guard = account_ref.lock().unwrap();

                verify(&account_guard, account_guard.init_amount, account_guard.amount);
            }
            println!("All checks passed.\n");

            for account in &accounts {
                println!("{:?}", account.as_ref().lock().unwrap());
            }

            println!("\n--------------------------------------------------\n");
        }

        if command == "x" {
            is_finished = true;
            continue;
        }
    }
}

fn generate_random_index(min_id: usize, max_id: usize) -> usize {
    rand::thread_rng().gen_range(min_id..max_id) 
}

fn generate_amount() -> u64 {
    rand::thread_rng().gen_range(1..=10000)
}

fn verify(account: &Account, init_amount: u64, check_amount: u64) {
    if account.log == "" {
        return;
    }

    let vec_logs = account.log.split(",").collect::<Vec<&str>>();
        
    let mut amount = init_amount;

    for log in vec_logs {
        let log = String::from(log);

        let op_log = log.split(":").collect::<Vec<&str>>();

        let elems = op_log[1].split("->").collect::<Vec<&str>>();

        let id_from = elems[0].parse::<u64>().unwrap();
        let amount_transf = elems[1].parse::<u64>().unwrap();
        let id_to = String::from(elems[2]).parse::<u64>().unwrap();

        if id_from == account.id {
            amount -= amount_transf;
        }
        else if id_to == account.id {
            amount += amount_transf;
        }
    }

    assert_eq!(amount, check_amount);
}