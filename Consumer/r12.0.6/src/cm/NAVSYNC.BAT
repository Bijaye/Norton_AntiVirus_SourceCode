cd c:\work
del . /qsxyz


@set p4client=cm_nav_consumer_trunk_cm_client

@p4 sync -f

@p4 label -o cm_navconsumer_9.00_98_%1 > temp.lab
@p4 label -i < temp.lab
@p4 labelsync -l cm_navconsumer_9.00_98_%1
@p4 label -o cm_navconsumer_9.00_98_%1 > temp.lab

@iff exist r:\cm\locklabel.py then
    @call python r:\cm\locklabel.py temp.lab
@endiff

@p4 label -i < temp.lab

cd c:\build\hold







