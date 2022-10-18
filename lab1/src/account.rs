#[derive(Debug)]
pub struct Account {
    pub iban: String,
    pub id: u64,
    pub amount: u64,
    pub log: String,
    pub init_amount: u64,
}

impl Account {
    pub fn new(iban: String, id: u64, amount: u64, log: String, init_amount: u64) -> Account {
        Account { iban, id, amount, log, init_amount }
    }
}