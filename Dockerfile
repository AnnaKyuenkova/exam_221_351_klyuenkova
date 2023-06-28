FROM ubuntu:18.04

# Автор: Клюенкова Анна Владимировна 221-351

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update
RUN apt-get install qt5-default -y
RUN apt-get install qtbase5-dev -y
RUN apt-get install qt5-qmake
RUN apt-get install build-essential -y

WORKDIR /221_351_Kluyenkova_Anna

COPY . .

RUN qmake mySer.pro && make

CMD ["./mySer", "Server started"]