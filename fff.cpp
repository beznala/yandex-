void MainWindow::on_pushButton_3_clicked()
{
    if(M_Accounts::m_accounts->readyStart && !M_Accounts::m_accounts->currentWork){
        M_Accounts::m_accounts->currentWork = true;
        M_Accounts::m_accounts->formatBin(ui->bin_list->text());
        M_Accounts::m_accounts->max_price = ui->max_price->text().toInt();
        M_Accounts::m_accounts->max_purch = ui->max_purch->text().toInt();
        M_Accounts::m_accounts->min_price = ui->min_price->text().toInt();


        int needed_threads = 1;
        int current_number = 50;
        while(M_Accounts::m_accounts->list.length() > current_number){
            needed_threads++;
            current_number += 50;
        }
        M_Thread::t_manager->setThreads(needed_threads);

        M_Thread::t_manager->startThreads();
        ui->label_10->setText("Work: Work is being performed");
    } else if(M_Accounts::m_accounts->currentWork) {
        ui->label_10->setText("Work: Work has already started");
    } else {
        ui->label_10->setText("Work: Check your cookies");
    }
}


void MainWindow::on_pushButton_clicked()
{
    if(M_Accounts::m_accounts->currentWork){
        M_Thread::t_manager->stop();
        ui->label_10->setText("Work: The work has been completed");
        M_Accounts::m_accounts->currentWork = false;

        if(M_Proxy::m_proxy->get__work())
            M_Proxy::m_proxy->stop__update();
    } else {
        ui->label_10->setText("Work: Work is not started");
    }
}

void MainWindow::on_base__chose_2_clicked()
{
    if(ui->auth_cookie->text().length() > 10 && ui->max_purch_2->text() > 10){
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL* curl;
        curl = curl_easy_init();
        std::string result;
        if(curl){
            curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_REVOKE_BEST_EFFORT | CURLSSLOPT_ALLOW_BEAST);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

            QString UserAgent = ui->max_purch_2->text();
            std::string UserAgentZ = UserAgent.toStdString();
            curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgentZ.c_str());
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 4);
            curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            struct curl_slist* chunk = NULL;
            QString save_cookie = ui->auth_cookie->text();
            QString cookie = "Cookie: " + ui->auth_cookie->text();
            chunk = curl_slist_append(chunk, cookie.toStdString().c_str());
            chunk = curl_slist_append(chunk, "Sec-Fetch-Dest: document");
            chunk = curl_slist_append(chunk, "Sec-Fetch-User: ?1");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Mode: navigate");
            chunk = curl_slist_append(chunk, "Sec-Fetch-Site: same-origin");
            chunk = curl_slist_append(chunk, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
            chunk = curl_slist_append(chunk, "Upgrade-Insecure-Requests: 1");
            chunk = curl_slist_append(chunk, "Cache-Control: max-age=0");
            chunk = curl_slist_append(chunk, "Connection: keep-alive");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_URL, "https://google.ru/");

            CURLcode res;
            res = curl_easy_perform(curl);
            if(res == CURLE_OK){
                if(result.find("Balance:") != std::string::npos){
                    QString balance = Pars("Balance: <strong class=\"\" style=\"font-size:14px;color:#32CD32\">", QString::fromStdString(result), " ") + " $";
                    ui->label_7->setText("Successful authorization");
                    ui->lines_label->setText(balance);
                    M_Accounts::m_accounts->user__cookie = save_cookie;
                    M_Accounts::m_accounts->readyStart = true;
                    M_Accounts::m_accounts->UserAgent = UserAgentZ;
                }
                else {
                    ui->label_7->setText("Authorization error");
                    M_Accounts::m_accounts->readyStart = false;
                }
            } else {
                ui->label_7->setText("Connection error");
                M_Accounts::m_accounts->readyStart = false;
            }
        }
        curl_easy_cleanup(curl);
        curl = NULL;
    } else if (ui->auth_cookie->text().length() < 10) {
       ui->label_7->setText("Check the box with the cookies");
    } else {
        ui->label_7->setText("Check the box with the User-Agent");
    }
}


QString MainWindow::Pars(QString T_, QString T, QString _T, QString regular){
    if(T_.length() == 0 || T.length() == 0 || T.length() == 0)
        return "";

    if(regular != "")
        T = T.mid(T.indexOf(regular) + regular.length(), T.length());

    int a = T.indexOf(T_);

    if(a == -1) return ""; else {
        a = a + T_.length();
    }

    T = T.mid(a, T.length());
    int b = T.indexOf(_T);

    if(b == -1) return "";

    return T.mid(0, b);
}