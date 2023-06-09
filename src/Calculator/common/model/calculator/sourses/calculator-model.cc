#include "../headers/calculator-model.h"

namespace s21 {

// Begin Class Calculator

const std::string CalculatorModel::Calculation::checkFunction(size_t &i) {
  std::string function{};
  while (std::isalpha(expression_[i])) {
    function += expression_[i];
    ++i;
  }
  if (this->operators_.find(function) == this->operators_.end()) {
    throw std::invalid_argument("error: the function does not exist");
  }
  if (function != "mod" && expression_[i] != '(') {
    throw std::invalid_argument("error: missing function argument");
  }
  return function;
}

const std::string CalculatorModel::Calculation::checkOperator(size_t &i) {
  std::string oper{};
  if (expression_[i] == '-' && unaryOperator(i)) {
    oper += "unary_minus";
  } else if (expression_[i] == '+' && unaryOperator(i)) {
    oper += "unary_plus";
  } else {
    oper += expression_[i];
  }
  ++i;
  if (this->operators_.find(oper) == this->operators_.end()) {
    throw std::invalid_argument("error: the operator does not exist");
  }
  return oper;
}

bool CalculatorModel::Calculation::unaryOperator(size_t i) noexcept {
  return (i == 0 || expression_[i - 1] == '(');
}

double CalculatorModel::Calculation::calculate(QString expression) {
  this->setExpression(expression);
  this->devideOnLexems();
  this->polishConverter();
  this->polishCalculate();
  return this->data;
}

std::string CalculatorModel::Calculation::getExpression() noexcept {
  return this->expression_;
};

double CalculatorModel::Calculation::getData() noexcept { return this->data; };

void CalculatorModel::Calculation::setExpression(QString expression) {
  if (expression.isEmpty() || expression.isNull())
    throw std::logic_error("incorrect expression");

  int count = std::count(expression.begin(), expression.end(), ')') +
              std::count(expression.begin(), expression.end(), '(');

  if ((count % 2)) throw std::logic_error("incorrect expression");

  this->expression_ =
      expression.toLower().simplified().replace(" ", "").toStdString();
};

void CalculatorModel::Calculation::devideOnLexems() {
  for (size_t i{}; i < expression_.size();) {
    if (std::isdigit(expression_[i])) {
      this->lexems_.push_back(numberValidation(i));
    } else if (std::isalpha(expression_[i])) {
      this->lexems_.push_back(checkFunction(i));
    } else {
      this->lexems_.push_back(checkOperator(i));
    }
  }
};

const std::string CalculatorModel::Calculation::numberValidation(size_t &i) {
  std::string number{};
  int check_of_dot{};
  while (std::isdigit(expression_[i]) || expression_[i] == '.') {
    number += expression_[i];
    ++i;
    if (expression_[i] == '.') {
      ++check_of_dot;
    }
  }
  if (check_of_dot > 1) {
    throw std::invalid_argument("error: too many dots in the number");
  }
  return number;
}

void CalculatorModel::Calculation::polishCalculate() {
  double number_one{};
  double number_two{};
  std::vector<double> tmp_result{};
  size_t i{};
  while (i < polish_notation_.size()) {
    if (std::isdigit(polish_notation_[i][0])) {
      tmp_result.push_back(std::stod(polish_notation_[i]));
    } else {
      size_t size_tmp_result = tmp_result.size();
      if (size_tmp_result >= 1) {
        number_two = tmp_result[size_tmp_result - 1];
        if (polish_notation_[i] == "unary_minus") {
          tmp_result[size_tmp_result - 1] = -number_two;
        } else if (polish_notation_[i] == "cos") {
          tmp_result[size_tmp_result - 1] = cos(number_two);
        } else if (polish_notation_[i] == "sin") {
          tmp_result[size_tmp_result - 1] = sin(number_two);
        } else if (polish_notation_[i] == "tan") {
          tmp_result[size_tmp_result - 1] = tan(number_two);
        } else if (polish_notation_[i] == "acos") {
          tmp_result[size_tmp_result - 1] = acos(number_two);
        } else if (polish_notation_[i] == "asin") {
          tmp_result[size_tmp_result - 1] = asin(number_two);
        } else if (polish_notation_[i] == "atan") {
          tmp_result[size_tmp_result - 1] = atan(number_two);
        } else if (polish_notation_[i] == "sqrt") {
          tmp_result[size_tmp_result - 1] = sqrt(number_two);
        } else if (polish_notation_[i] == "ln") {
          tmp_result[size_tmp_result - 1] = log(number_two);
        } else if (polish_notation_[i] == "log") {
          tmp_result[size_tmp_result - 1] = log10(number_two);
        }
      }
      std::string pattern("+-*/^mod");
      if (pattern.find(polish_notation_[i]) != std::string::npos) {
        if (size_tmp_result >= 2) {
          number_one = tmp_result[size_tmp_result - 2];
          if (polish_notation_[i] == "+") {
            tmp_result[size_tmp_result - 2] = number_one + number_two;
          } else if (polish_notation_[i] == "-") {
            tmp_result[size_tmp_result - 2] = number_one - number_two;
          } else if (polish_notation_[i] == "*") {
            tmp_result[size_tmp_result - 2] = number_one * number_two;
          } else if (polish_notation_[i] == "/") {
            tmp_result[size_tmp_result - 2] = number_one / number_two;
          } else if (polish_notation_[i] == "^") {
            tmp_result[size_tmp_result - 2] = std::pow(number_one, number_two);
          } else if (polish_notation_[i] == "mod") {
            tmp_result[size_tmp_result - 2] = std::fmod(number_one, number_two);
          }
          tmp_result.pop_back();
        } else {
          throw std::invalid_argument("error: incorrect expression");
        }
      }
    }
    ++i;
  }
  if (tmp_result.size() != 1) {
    throw std::invalid_argument("error: incorrect expression");
  }
  this->data = tmp_result[0];
}

void CalculatorModel::Calculation::polishConverter() {
  std::vector<std::string> operator_stack{};
  for (const auto &str : this->lexems_) {
    if (std::isdigit(str[0])) {
      polish_notation_.push_back(str);
    } else if (str == "(") {
      operator_stack.push_back(str);
    } else if (str == ")") {
      while (operator_stack.back() != "(") {
        polish_notation_.push_back(operator_stack.back());
        operator_stack.pop_back();
      }
      operator_stack.pop_back();
    } else {
      if (!operator_stack.empty()) {
        int priority_in_expression = this->operators_.find(str)->second;
        int priority_in_stack =
            this->operators_.find(operator_stack.back())->second;
        while (priority_in_stack >= priority_in_expression) {
          polish_notation_.push_back(operator_stack.back());
          operator_stack.pop_back();
          if (operator_stack.empty()) {
            break;
          }
          priority_in_stack =
              this->operators_.find(operator_stack.back())->second;
        }
      }
      operator_stack.push_back(str);
    }
  }
  while (!operator_stack.empty()) {
    polish_notation_.push_back(operator_stack.back());
    operator_stack.pop_back();
  }
}

void CalculatorModel::Calculation::clear() noexcept {
  this->lexems_.clear();
  this->polish_notation_.clear();
  this->expression_ = "";
  this->data = 0;
}

// End Class Calculator

// Begin Class CalculatorModel

QVector<CalculatorModel::Calculation>
CalculatorModel::getCalculations() noexcept {
  return this->calculations;
};

CalculatorModel::Calculation CalculatorModel::getLastCalculation() {
  return this->calculations.back();
};

void CalculatorModel::setCalculations(
    QVector<CalculatorModel::Calculation> calculations) noexcept {
  this->calculations = calculations;
};

void CalculatorModel::setLastCalculation(Calculation calculation) noexcept {
  this->calculations.push_back(calculation);
};

QVector<CalculatorModel::CreditCalculation>
CalculatorModel::getCreditCalculation() noexcept {
  return this->credit_calculations;
};

CalculatorModel::CreditCalculation CalculatorModel::getLastCreditCalculation() {
  return this->credit_calculations.back();
};

void CalculatorModel::setCreditCalculations(
    QVector<CalculatorModel::CreditCalculation> credit_calculations) noexcept {
  this->credit_calculations = credit_calculations;
};
void CalculatorModel::setLastCreditCalculation(
    CalculatorModel::CreditCalculation credit_calculations) noexcept {
  this->credit_calculations.push_back(credit_calculations);
};

void CalculatorModel::reset() {
  this->calculations.clear();
  this->credit_calculations.clear();
};

// End Class CalculatorModel

// Begin Class CommonType

void setMonthlyPayment(double monthly_payment);
void setCreditOverpayment(double credit_overpayment);
void setTotalPayment(double total_payment);

double getMonthlyPayment() noexcept;
double getCreditOverpayment() noexcept;
double getTotalPayment() noexcept;

void calculate(double total_loan_amount, double period, double interest_rate);

void CalculatorModel::CreditCalculation::CommonType::setMonthlyPayment(
    double monthly_payment) {
  this->monthly_payment_ = monthly_payment;
}

void CalculatorModel::CreditCalculation::CommonType::setCreditOverpayment(
    double credit_overpayment) {
  this->credit_overpayment_ = credit_overpayment;
}

void CalculatorModel::CreditCalculation::CommonType::setTotalPayment(
    double total_payment) {
  this->total_payment_ = total_payment;
}

double
CalculatorModel::CreditCalculation::CommonType::getMonthlyPayment() noexcept {
  return this->monthly_payment_;
}

double CalculatorModel::CreditCalculation::CommonType::
    getCreditOverpayment() noexcept {
  return this->credit_overpayment_;
}

double
CalculatorModel::CreditCalculation::CommonType::getTotalPayment() noexcept {
  return this->total_payment_;
}

std::vector<double>
CalculatorModel::CreditCalculation::CommonType::validateExpressions(
    QString total_loan_amount, QString period, QString interest_rate) {
  if (total_loan_amount.isEmpty() || period.isEmpty() ||
      interest_rate.isEmpty())
    throw std::logic_error("conversion's numbers error");

  bool check_str = 0;

  total_loan_amount.replace(QChar(','), QLatin1String("."),
                            Qt::CaseInsensitive);
  period.replace(QChar(','), QLatin1String("."), Qt::CaseInsensitive);
  interest_rate.replace(QChar(','), QLatin1String("."), Qt::CaseInsensitive);

  double total_loan_res = total_loan_amount.toDouble(&check_str);
  if (!check_str) throw std::logic_error("conversion's numbers error");
  double period_res = period.toDouble(&check_str);
  if (!check_str) throw std::logic_error("conversion's numbers error");
  double interest_rate_res = interest_rate.toDouble(&check_str);
  if (!check_str) throw std::logic_error("conversion's numbers error");

  if (total_loan_res <= 0 || period_res <= 0 || interest_rate_res <= 0)
    throw std::logic_error("conversion's numbers error");

  std::vector<double> res;
  res.push_back(total_loan_res);
  res.push_back(period_res);
  res.push_back(interest_rate_res);

  return res;
}

// End Class CommonType

// Begin Class Annuity

void CalculatorModel::CreditCalculation::Annuity::calculate(
    QString total_loan_amount, QString period, QString interest_rate) {
  try {
    std::vector<double> variables =
        this->validateExpressions(total_loan_amount, period, interest_rate);
    this->calculateCredit(variables);
  } catch (...) {
    throw std::logic_error("credit calculation error");
  }
}

void CalculatorModel::CreditCalculation::Annuity::calculateCredit(
    std::vector<double> variables) {
  if (variables.empty() && variables.size() < 3)
    throw std::logic_error("empty error");

  double S = variables[0];
  double N = variables[1];
  double R = variables[2] / 12.0 / 100.0;

  double res_mounth = S * (R * pow(1 + R, N)) / (pow(1 + R, N) - 1);
  res_mounth = round(res_mounth * 100.0) / 100.0;
  double res_overpay = res_mounth * N - S;
  res_overpay = round(res_overpay * 100.0) / 100.0;
  double res_common_pay = res_overpay + S;
  res_common_pay = round(res_common_pay * 100) / 100;

  this->setMonthlyPayment(res_mounth);
  this->setTotalPayment(res_common_pay);
  this->setCreditOverpayment(res_overpay);
}

// End Class Annuity

// Begin Class Differential

void CalculatorModel::CreditCalculation::Differential::calculate(
    QString total_loan_amount, QString period, QString interest_rate) {
  try {
    std::vector<double> variables =
        this->validateExpressions(total_loan_amount, period, interest_rate);
    this->calculateCredit(variables);
  } catch (...) {
    throw std::logic_error("credit calculation error");
  }
}

void CalculatorModel::CreditCalculation::Differential::calculateCredit(
    std::vector<double> variables) {
  if (variables.empty() && variables.size() < 3)
    throw std::logic_error("empty error");

  double S = variables[0];
  double N = variables[1];
  double R = variables[2] / 100.0 / 12.0;
  double D = S / N;
  double res_overpay = 0;

  for (int i = 1; i <= N; i++) {
    double q = S / N;
    double tmp = D + (S - D * (i - 1)) * R;
    tmp = round(tmp * 100) / 100;
    this->month_payments_.push_back(tmp);

    res_overpay += tmp - q;
  }

  double res_common_pay = res_overpay + S;
  res_common_pay = round(res_common_pay * 100) / 100;

  this->setTotalPayment(res_common_pay);
  this->setCreditOverpayment(res_overpay);
}

std::vector<double>
CalculatorModel::CreditCalculation::Differential::getMonthPayments() {
  return this->month_payments_;
};

void CalculatorModel::CreditCalculation::Differential::setMonthPayments(
    std::vector<double> month_payments) {
  this->month_payments_ = month_payments;
};

// End Class Differential

// Begin Class DepositCalculation

void CalculatorModel::DepositCalculation::calculate(
    const struct InputData Data) {
  std::map<QString, double> variables;
  std::map<QString, std::vector<double>> lists;
  try {
    variables = this->validateExpressions(Data);
    lists = this->validateList(Data);
    this->calculateDeposit(variables, lists, Data);
  } catch (std::exception &e) {
    variables.clear();
    lists.clear();
    throw std::logic_error(e.what());
  }
  variables.clear();
  lists.clear();
};

std::map<QString, double>
CalculatorModel::DepositCalculation::validateExpressions(
    const struct InputData Data) {
  std::map<QString, double> res;

  try {
    QString deposit_amount = this->validateNullorEmpty(Data.getDepositAmount());
    res.insert(std::pair<QString, double>(
        "deposit_amount", this->changeToDouble(deposit_amount)));

    QString placement_term = this->validateNullorEmpty(Data.getPlacementTerm());
    res.insert(std::pair<QString, double>(
        "placement_term", this->changeToDouble(placement_term)));

    QString interest_rate = this->validateNullorEmpty(Data.getInterestRate());
    res.insert(std::pair<QString, double>("interest_rate",
                                          this->changeToDouble(interest_rate)));

    QString tax_rate = this->validateNullorEmpty(Data.getTaxRate());
    res.insert(
        std::pair<QString, double>("tax_rate", this->changeToDouble(tax_rate)));

    QString payment_frequency =
        this->validateNullorEmpty(Data.getPaymentFrequency());
    res.insert(std::pair<QString, double>(
        "payment_frequency", this->changeToDouble(payment_frequency)));

    QString interest_capitalization =
        this->validateNullorEmpty(Data.getInterestCapitalization());
    res.insert(std::pair<QString, double>(
        "interest_capitalization",
        this->changeToDouble(interest_capitalization)));
  } catch (std::exception &e) {
    res.clear();
    throw e;
  }

  return res;
};

std::map<QString, std::vector<double>>
CalculatorModel::DepositCalculation::validateList(const struct InputData Data) {
  std::vector<QString> replenishment_list = Data.getReplenishmentList();
  std::vector<QString> partial_withdrawals_list =
      Data.getPartialWithdrawalsList();

  if (replenishment_list.empty() || partial_withdrawals_list.empty()) {
    throw std::invalid_argument("error: incorrect expression");
  }

  std::map<QString, std::vector<double>> res;

  try {
    res.insert(std::pair<QString, std::vector<double>>(
        "replenishment_list", this->changeToDouble(replenishment_list)));
    res.insert(std::pair<QString, std::vector<double>>(
        "partial_withdrawals_list",
        this->changeToDouble(partial_withdrawals_list)));
  } catch (std::exception &e) {
    res.clear();
    throw e;
  }

  return res;
}

QString CalculatorModel::DepositCalculation::validateNullorEmpty(QString str) {
  if (str.isEmpty() || str.isNull()) {
    throw std::invalid_argument("error: incorrect expression");
  }

  return str;
};

void CalculatorModel::DepositCalculation::calculateDeposit(
    const std::map<QString, double> variables,
    const std::map<QString, std::vector<double>> lists,
    const struct InputData &Data) {
  if (variables.size() || lists.size())
    throw std::logic_error("maps are empty");

  if (Data.getStatusCapitalisation()) {
    this->simplePercent(variables, lists, Data);
  }

  //    сумма вклада
  //    срок размещения
  //    процентная ставка
  //    налоговая ставка 13 процентов, если больше 12.5 процентов
  //    периодичность выплат: 1 раз в год
  //    капитализация процентов: 1 раз в год
  //    список пополнений: 1 раз в год
  //    список частичных снятий: 1 раз в год

  //начисленные проценты
  //сумма налога
  //сумма на вкладе к концу срока
};

void CalculatorModel::DepositCalculation::simplePercent(
    const std::map<QString, double> variables,
    const std::map<QString, std::vector<double>> lists,
    const struct InputData &Data) {
  double P = variables.find("deposit_amount")->second;
  double t = variables.find("placement_term")->second;
  double I = variables.find("interest_rate")->second;
  double N = variables.find("tax_rate")->second;
  double payment_frequency = variables.find("payment_frequency")->second;
  double interest_capitalization =
      variables.find("interest_capitalization")->second;
  std::vector<double> replenishment_list =
      lists.find("replenishment_list")->second;
  std::vector<double> partial_withdrawals_list =
      lists.find("partial_withdrawals_list")->second;

  double S = (P * I * t / 365) / 100;
  S = round(S * 100) / 100;

  this->setEndSumDeposit(S);
  this->setAmountTaxation(0);
  this->setAccruedInterest(P + S);
}

double CalculatorModel::DepositCalculation::changeToDouble(QString num) {
  num.replace(QChar(','), QLatin1String("."), Qt::CaseInsensitive);
  num.replace(QChar(','), QLatin1String("."), Qt::CaseInsensitive);
  num.replace(QChar(','), QLatin1String("."), Qt::CaseInsensitive);

  bool check_str = 0;

  double res = num.toDouble(&check_str);

  if (!check_str) throw std::logic_error("conversion's numbers error");

  if (res <= 0) throw std::logic_error("conversion's numbers error");

  return res;
}

std::vector<double> CalculatorModel::DepositCalculation::changeToDouble(
    const std::vector<QString> num) {
  std::vector<double> res;

  for (auto it = num.begin(); it != num.end(); it++) {
    try {
      QString tmp = this->validateNullorEmpty(*it);
      res.push_back(changeToDouble(tmp));
    } catch (std::exception &e) {
      res.clear();
      throw e;
    }
  }

  return res;
};

// End Class DepositCalculation

};  //  namespace s21
