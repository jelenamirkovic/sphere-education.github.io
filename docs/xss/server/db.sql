CREATE DATABASE forum;
USE forum;

CREATE TABLE users (
user_id     INT(8) NOT NULL AUTO_INCREMENT,
user_name   VARCHAR(30) NOT NULL,
user_pass   VARCHAR(255) NOT NULL,
user_email  VARCHAR(255) NOT NULL,
user_date   DATETIME NOT NULL,
user_level  INT(8) NOT NULL,
UNIQUE INDEX user_name_unique (user_name),
PRIMARY KEY (user_id)
);

CREATE TABLE categories (
cat_id          INT(8) NOT NULL AUTO_INCREMENT,
cat_name        VARCHAR(255) NOT NULL,
cat_description     VARCHAR(255) NOT NULL,
UNIQUE INDEX cat_name_unique (cat_name),
PRIMARY KEY (cat_id)
);

CREATE TABLE topics (
topic_id        INT(8) NOT NULL AUTO_INCREMENT,
topic_subject       VARCHAR(255) NOT NULL,
topic_date      DATETIME NOT NULL,
topic_cat       INT(8) NOT NULL,
topic_by        INT(8) NOT NULL,
PRIMARY KEY (topic_id)
);

CREATE TABLE posts (
post_id         INT(8) NOT NULL AUTO_INCREMENT,
post_content        TEXT NOT NULL,
post_date       DATETIME NOT NULL,
post_topic      INT(8) NOT NULL,
post_by     INT(8) NOT NULL,
PRIMARY KEY (post_id)
);

ALTER TABLE topics ADD FOREIGN KEY(topic_cat) REFERENCES categories(cat_id) ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE topics ADD FOREIGN KEY(topic_by) REFERENCES users(user_id) ON DELETE RESTRICT ON UPDATE CASCADE;

ALTER TABLE posts ADD FOREIGN KEY(post_topic) REFERENCES topics(topic_id) ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE posts ADD FOREIGN KEY(post_by) REFERENCES users(user_id) ON DELETE RESTRICT ON UPDATE CASCADE;
INSERT INTO users(user_name, user_pass, user_email, user_date, user_level) VALUES ("root", SHA1("root"), "root@slothsunlimited.com", NOW(), 1);
INSERT INTO users(user_name, user_pass, user_email, user_date, user_level) VALUES ("SlothFromHoth", SHA1("sloth1"), "sloth@slothunlimited.com", NOW(), 0);
INSERT INTO users(user_name, user_pass, user_email, user_date, user_level) VALUES ("TheSleuthSloth", SHA1("sloth2"), "sleuth@slothsunlumited.com", NOW(), 0);
INSERT INTO users(user_name, user_pass, user_email, user_date, user_level) VALUES ("Slowbro", SHA1("sloth3"), "slowbro@slothsunlimited.com", NOW(), 0);
INSERT INTO users(user_name, user_pass, user_email, user_date, user_level) VALUES ("SpeedySteve", SHA1("sloth4"), "speedy@slothsunlimited.com", NOW(), 0);
INSERT INTO categories(cat_name, cat_description) VALUES ("Sloths", "All things sloths.");
INSERT INTO categories(cat_name, cat_description) VALUES ("Cecropia", "The best tree in existence.");
INSERT INTO categories(cat_name, cat_description) VALUES ("The Branch", "Come and hang out!");
INSERT INTO topics(topic_subject, topic_date, topic_cat, topic_by) VALUES ("Ants!", NOW(), 1, 2);
INSERT INTO topics(topic_subject, topic_date, topic_cat, topic_by) VALUES ("Somebody call the bomb squad...", NOW(), 2, 3);
INSERT INTO topics(topic_subject, topic_date, topic_cat, topic_by) VALUES ("low key party at my branch", NOW(), 3, 4);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Gonna have a chill party at my branch, probably eat some leaves... slowly. Maybe take a nap, and then hang for a bit. Reply if you're interested.", NOW(), 3, 4);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("I'm down", NOW(), 3, 2);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("I'll bring leaves. Someone should get some catepillars too. We'll make wraps.", NOW(), 3, 3);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Solid. I'm gonna take my second after-lunch nap, then lets do this. Head over whenever.", NOW(), 3, 4);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Hey guys! Can I come?", NOW(), 3, 5);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Darnit Steve, we just want to have a chill time without anyone crawling across our branches at mach 7.", NOW(), 3, 4);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Aww give me a chance! Also I crashed my anteater into my branch. Can I borrow yours until mine gets fixed?", NOW(), 3, 5);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("This is why we can't have nice things steve.", NOW(), 3, 4);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("They're everywhere! Somebody help!!", NOW(), 1, 2);
INSERT INTO posts(post_content, post_date, post_topic, post_by) VALUES ("Because these leaves are exploding with flavor!", NOW(), 2, 3);
