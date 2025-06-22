psql -U postgres -c "CREATE DATABASE textdb;"
psql -U postgres -d textdb -f build.sql
psql -U postgres -c "CREATE USER textuser WITH PASSWORD 'secure_password';"
psql -U postgres -c "GRANT ALL PRIVILEGES ON DATABASE textdb TO textuser;"
