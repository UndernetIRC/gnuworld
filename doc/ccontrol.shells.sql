CREATE TABLE ShellCompanies (
        id SERIAL,
        name VARCHAR(200) UNIQUE NOT NULL,
        active int4 NOT NULL DEFAULT 1,
        addedby VARCHAR(200) NOT NULL,
        addedon int4 NOT NULL,
        lastmodby VARCHAR(200) NOT NULL,
        lastmodon int4 NOT NULL,
        maxlimit int4 NOT NULL
        );

CREATE TABLE ShellNetblocks (
        companyid int4 NOT NULL,        
        cidr VARCHAR(20) NOT NULL,
        addedby VARCHAR(200) NOT NULL,
        addedon int4 NOT NULL
        );
