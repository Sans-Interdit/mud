#include "classes.h"

Entite::Entite(std::string filePath, bool const giveHitBox = true)
{
    m_img.loadFromFile(filePath);
    this->setTexture(m_img);
    if (giveHitBox)
    {
        setHitbox();
    }
}

sf::FloatRect Entite::getHitbox()
{
    return m_hitbox;
}

void Entite::setImg(std::string const& filepath)
{
    m_imgName = filepath;
    m_img.loadFromFile(m_imgName);
    setTexture(m_img);
}

sf::Texture Entite::getImg() { return m_img; }

std::string Entite::getImgName() { return m_imgName; }

void Entite::setHitbox() { m_hitbox = getGlobalBounds(); }

Plateforme::Plateforme(float const x, float const y, std::string type)
{
    this->m_img.loadFromFile("Sprites/" + type + ".png");
    this->setTexture(m_img);
    this->setPosition(x, y);
    setHitbox();
}

void Unit::hit(int const dmg, double push)
{
    m_hp -= dmg;
    m_vecteurX = push;
    m_hitTime = 0.3;
    m_ptrGroup->ptrAtt->erase(&m_attaque[m_numAtt]);
    m_attaque[m_numAtt].setDelay(-m_attaque[m_numAtt].getDelayStatic());
    m_attAnimTmp = m_attaque[m_numAtt].getAvTmp();
}

EntityLists* Unit::getPtrGroup() { return m_ptrGroup; }

bool Unit::getADroite() { return m_aDroite; }

void Unit::setADroite(bool d) { m_aDroite = d; }

Stats Unit::getStat() { return m_stat; }

int Unit::getNumAtt() { return m_numAtt; }

void Unit::setNumAtt(int val) { m_numAtt = val; }

void SolUnit::physique()
{
    move(static_cast<float>(m_vecteurX), 0);
    if (m_vecteurX > 0)
    {
        m_vecteurX -= 0.5;
    }
    else if (m_vecteurX < 0)
    {
        m_vecteurX += 0.5;
    }
    /*
    else
    {
        m_vecteurX = 0;
    }
    */
    if (!m_auSol)
    {
        double posY{ -35 * (m_tmpSaut * m_tmpSaut) / 2 + m_vecteurY };
        move(0, static_cast<float>(-posY));
        m_tmpSaut += 0.016;
    }


    setHitbox();

    sf::FloatRect zonePied(m_hitbox.left, m_hitbox.top + m_hitbox.height - 5, m_hitbox.width, 5);
    m_auSol = false;

    for (Entite* plateforme : *m_ptrGroup->ptrPF)
    {
        sf::FloatRect const hitboxPF{ plateforme->getHitbox() };
        sf::Vector2f diffPos { m_avPos - getPosition() };
        bool axePF{ m_avPos.x + m_hitbox.width > plateforme->getPosition().x && m_avPos.x < plateforme->getPosition().x + hitboxPF.width };
        if (hitboxPF.intersects(zonePied) && axePF)
        {

            //setPosition(getPosition().x, plateforme->getPosition().y - m_hitbox.height);
            while (hitboxPF.top < getPosition().y + m_hitbox.height /* + diffPos.y / 5*/ - 5)
            {
                move(0, diffPos.y / 5);
            }
            m_auSol = true;
            m_tmpSaut = 0;
            //m_avPos.y = getPosition().y;
            m_vecteurY = -3;
        }
        else if (m_hitbox.intersects(hitboxPF))
        {
            if (!m_auSol && !axePF)
            {
                if (hitboxPF.intersects(sf::FloatRect(m_avPos.x, m_hitbox.top, m_hitbox.width, m_hitbox.height)))
                {
                    std::cout << 'd';
                }
                //while (m_hitbox.intersects(hitboxPF))
                //{
                    setPosition(m_avPos.x  /* + diff.x * 0.2*/, getPosition().y);
                //}
            }
            else
            {
                setPosition(m_avPos.x/* + diff.x * 0.2*/, getPosition().y);
                m_vecteurY = 0;
                m_vecteurX = 0;
                //break;
            }
        }
    }
    m_avPos = getPosition();
}

Ennemie::Ennemie(EntityLists* drawable, float x, float y)
{
    setImg("Sprites/testBad.png");
    setPosition(x, y);
    m_ptrGroup = drawable;
    m_ptrGroup->ptrEnn->insert(this);
    m_hp = 500;
    m_stat = { 150, 20, 5, std::vector<double>{ 2 } };
    m_attaque = { Attaque(this, AllAttTypes::cac, "Sprites/Attaque1.png", 0.5, 1.0, 10, 1) };
}

void Ennemie::attack()
{
    sf::FloatRect zoneAtt = m_aDroite ? sf::FloatRect{getPosition().x, getPosition().y - m_hitbox.height / 4, static_cast<float>(m_hitbox.width * 3), static_cast<float>(m_hitbox.height * 1.5)} : sf::FloatRect{ getPosition().x - m_hitbox.width * 3, getPosition().y - m_hitbox.height / 4,static_cast<float>(m_hitbox.width * 3), static_cast<float>(m_hitbox.height * 1.5) };
    if (zoneAtt.intersects(m_ptrGroup->perso->getHitbox()))
    {
        if (m_attaque[m_numAtt].getDelay() < -m_stat.AS[m_numAtt])
        {
            m_attAnimTmp = m_attaque[m_numAtt].getAvTmp();
            m_attaque[m_numAtt].setDelay(m_attaque[m_numAtt].getDelayStatic());
        }
    }
    if (m_attAnimTmp <= 0)
    {
        if (m_attaque[m_numAtt].getDelay() == m_attaque[m_numAtt].getDelayStatic())
        {
            m_vecteurY = (-35 * (m_tmpSaut * m_tmpSaut) / 2 + m_vecteurY > 0) ? 1 : -1;
            m_tmpSaut = 0;
            m_attaque[m_numAtt].spawn();
        }
        else
        {
            m_attaque[m_numAtt].update();
        }
    }
    else
    {
        m_attAnimTmp -= 0.016;
    }
}

void Ennemie::run()
{
    if (getPosition().x <= 300)
    {
        m_aDroite = true;
    }
    else if (getPosition().x >= 600)
    {
        m_aDroite = false;
    }
    if (m_aDroite)
    {
        move(2, 0);
        m_tmp += 0.016;
    }
    else
    {
        move(-2, 0);
        m_tmp -= 0.016;
    }
}

void Ennemie::update()
{
    if (m_hp <= 0)
    {
        m_ptrGroup->ptrAtt->erase(&m_attaque[m_numAtt]);
        m_ptrGroup->ptrEnn->erase(this);
    }
    attack();
    if (m_attaque[m_numAtt].getDelay() <= 0)
    {
        if (m_hitTime <= 0)//bump gauche ou droite en fonction signe
        {
            run();
        }
        else
        {
            m_hitTime -= 0.016;
        }
    }
    physique();
}

std::string Ennemie::getType() { return "Ennemie"; }

//------------------------------------------------------------PJ------------------------------------------------------------------

PJ::PJ(EntityLists* drawable, int p)
{
    switch (p)
    {
    case (1):
        setImg("Sprites/billy.png");
        m_stat = { 150, 20, 5, {0.5,0.3,0.3 } };
        m_attaque = { Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.20, 1, 3), Attaque(this, AllAttTypes::cac, "Sprites/attaque2.png", 0.25, 1.2, 3), Attaque(this, AllAttTypes::cac, "Sprites/attaque3.png", 0.30, 1.5, 10, 0.25) };
        break;
    case (2):
        setImg("Sprites/tanky.png");
        m_stat = { 200, 30, 4, {0.5, 0.5, 0.5} };
        m_attaque = { Attaque(this, AllAttTypes::dist, "Sprites/carreau.png", 0.5, 1.2, 5), Attaque(this, AllAttTypes::dist, "Sprites/carreau.png", 0.25, 1.2, 5), Attaque(this, AllAttTypes::dist, "Sprites/carreau.png", 0.25, 1.2, 5) };
        break;
    case (3):
        setImg("Sprites/slimy.png");
        m_stat = { 50, 5, 5, {0.3,0.3,0.3} };
        m_attaque = { Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5), Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5), Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5) };
        break;
    default:
        setImg("Sprites/billy.png");
        m_stat = { 5000, 5, 5, {0.2,0.2,0.7} };
        m_attaque = { Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5), Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5), Attaque(this, AllAttTypes::cac, "Sprites/attaque1.png", 0.25, 1.2, 5) };
    }
    setPosition(sf::Vector2f(200, 200));
    m_hp = m_stat.maxHP;
    m_ptrGroup = drawable;
    m_dmgRect.setFillColor(sf::Color::Transparent);
    m_dmgRect.setOutlineColor(sf::Color::Red);
    m_dmgRect.setOutlineThickness(15);
    m_dmgRect.setPosition(getPosition().x + 15, getPosition().y + 15);
}


void PJ::attack()
{
    if (m_attaque[m_numAtt].getDelay() < -m_stat.AS[m_numAtt])
    {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::L) && !m_attHold) || m_attChain)
        {
            m_attAnimTmp = m_attaque[m_numAtt].getAvTmp();
            m_attaque[m_numAtt].setDelay(m_attaque[m_numAtt].getDelayStatic());
            m_attChain = false;
            m_attHold = true;
        }
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::L))
    {
        m_attHold = false;
    }
    else if ((m_attaque[m_numAtt].getDelay() <= 0 || m_numAtt != m_attaque.size() - 1) && m_attaque[m_numAtt].getDelay() >= -m_stat.AS[m_numAtt] && !m_attHold)
    {
        m_attChain = true;
    }
    //if (m_attaque[m_numAtt].getDelay() < 0) { m_attAnimTmp = 0; }
    if (m_attAnimTmp <= 0)
    {
        if (m_attaque[m_numAtt].getDelay() == m_attaque[m_numAtt].getDelayStatic())
        {
            m_vecteurY = (-35 * (m_tmpSaut * m_tmpSaut) / 2 + m_vecteurY > 0) ? 1 : -1;
            m_tmpSaut = 0;
            m_attaque[m_numAtt].spawn();
        }
        else
        {
            m_attaque[m_numAtt].update();
            if (m_attaque[m_numAtt].getDelay() < -0.3 - m_stat.AS[m_numAtt] && m_numAtt != 0)
            {
                m_attaque[m_numAtt].setDelay(0);
                m_numAtt = 0;
                m_attaque[0].setDelay(-m_stat.AS[0]);
            }
            if (m_imgCoord.x != 0) { m_imgCoord.x = 0; }
        }
    }
    else
    {
        if (m_imgCoord.x != 1) { m_imgCoord.x = 1; }
        m_attAnimTmp -= 0.016;
    }
}

void PJ::run()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        m_vecteurX = m_stat.vit;
        m_aDroite = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        m_vecteurX = -m_stat.vit;
        m_aDroite = false;
    }
}

void PJ::saut()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        //if (!m_sbMaintenue)
        {
            if (m_auSol&& !m_sbMaintenue)
            {
                m_vecteurY = 7;
                m_auSol = false;
                m_sbMaintenue = true;
            }
            else if (m_doubleSaut&& !m_sbMaintenue)
            {
                m_tmpSaut = 0;
                m_vecteurY = 4;
                m_doubleSaut = false;
            }
        }
    }
    else
    {
        m_sbMaintenue = false;
    }
}

void PJ::spe()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && m_speTmp > 0.4)
    {
        m_speTmp = 0;
    }
    else if (m_speTmp < 0.15)
    {
        if (m_aDroite)
        {
            move(17, 0);
        }
        else
        {
            move(-17, 0);
        }
    }
    m_speTmp += 0.016;
}

void PJ::update()
{
    setTextureRect(sf::IntRect(m_imgCoord.x * 40, int(!m_aDroite) * 120, 40, 120));
    if (!m_auSol && !m_sbMaintenue && -35 * (m_tmpSaut * m_tmpSaut) / 2 + m_vecteurY > 0 && m_doubleSaut)
    {
        m_vecteurY -= 1;
    }
    if (m_auSol)
    {
        m_doubleSaut = true;
    }
    /*
    if (slashing ^ dashing)
    {
        spe();
    }
    */
    bool const slashing{ m_attaque[m_numAtt].getDelay() > 0 };
    bool const dashing{ m_speTmp < 0.15 };
    if (!slashing && m_speTmp > 0.25)
    {
        saut();
        if (m_hitTime <= 0)
        {
            run();
            attack();
            m_ptrGroup->hud->erase(&m_dmgRect);
        }
        else
        {
            m_ptrGroup->hud->insert(&m_dmgRect);
            m_hitTime -= 0.016;
        }
    }
    physique();
}

jointVar PJ::resetVar()
{
    m_numAtt = 0 ;
    m_attAnimTmp = 0;
    m_attChain = 0;
    m_attaque[m_numAtt].reset();
    return { getPosition(),m_vecteurX,m_vecteurY,m_tmpSaut,m_aDroite };
}

void PJ::recoverVar(jointVar vars)
{
    setPosition(vars.pos);
    m_avPos = getPosition();
    m_vecteurX = vars.vecX;
    m_vecteurY = vars.vecY;
    m_tmpSaut = vars.tmpSaut;
    m_aDroite = vars.aDroite;
    m_doubleSaut = false;
    m_auSol = false;
}

std::string PJ::getType() { return "PJ"; }

int PJ::getHP() { return m_hp; }

Attaque::Attaque(Unit* joueur, AllAttTypes type, std::string filepath, double delay, double multiplier, double knockback, double avTmp) : m_delayStatic(delay), m_multiplier(multiplier), m_avTmp(avTmp), m_type(type), m_ptrPerso(joueur), m_knockback(knockback), m_delay(0)
{
    setImg(filepath);
    setHitbox();
    if (m_type == AllAttTypes::cac)
    {
        setOrigin(0, 25);
    }
}

void Attaque::spawn()
{
    if (m_type == AllAttTypes::dist)
    {
        if (m_ptrPerso->getADroite())
        {
            setPosition(m_ptrPerso->getPosition().x, m_ptrPerso->getPosition().y + 50);
        }
        else
        {
            setPosition(m_ptrPerso->getPosition().x - m_hitbox.width + m_ptrPerso->getHitbox().width, m_ptrPerso->getPosition().y + 50);
        }
    }
    setImg(m_imgName);//je ne ais pas pq c'est n�c�ssaire
    m_ptrPerso->getPtrGroup()->ptrAtt->insert(this);
    update();
}

void Attaque::Cac()
{
    if (m_delay > 0.02)
    {
        if (m_ptrPerso->getADroite())
        {
            setPosition(m_ptrPerso->getPosition().x, m_ptrPerso->getPosition().y);
        }
        else
        {
            setPosition(m_ptrPerso->getPosition().x - m_hitbox.width + m_ptrPerso->getHitbox().width, m_ptrPerso->getPosition().y);
        }
        setHitbox();
        if (m_ptrPerso->getType() == "Ennemie")//Tres mal r�alis� - a refaire plus tard
        {
            Unit* p{ m_ptrPerso->getPtrGroup()->perso };
            if (m_hitbox.intersects(p->getHitbox()) && !std::any_of(m_lstHit.begin(), m_lstHit.end(), [p](Unit* unit) {return unit == p; }))
            {
                p->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), m_ptrPerso->getADroite() ? m_knockback : -m_knockback);
                m_lstHit.insert(p);
            }
        }
        else if (m_ptrPerso->getType() == "PJ")
        {
            for (Unit* ennemie : *m_ptrPerso->getPtrGroup()->ptrEnn)
            {
                if (m_hitbox.intersects(ennemie->getHitbox()))
                {
                    if (!std::any_of(m_lstHit.begin(), m_lstHit.end(), [ennemie](Unit* unit) {return unit == ennemie; }))
                    {
                        ennemie->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), m_ptrPerso->getADroite() ? m_knockback : -m_knockback);
                        m_lstHit.insert(ennemie);
                    }
                }
            }
        }
    }
    else if (m_delay > 0)
    {
        reset();
        m_ptrPerso->setNumAtt(static_cast<uint16_t>(m_ptrPerso->getStat().AS.size() - 1) > m_ptrPerso->getNumAtt() ? m_ptrPerso->getNumAtt() + 1 : 0);
    }
    m_delay -= 0.016;
}

void Attaque::Dist()
{
    if (m_delay > 0.02)
    {
        if (m_ptrPerso->getADroite())
        {
            move(10, 0);
        }
        else
        {
            move(-10, 0);
        }
        setHitbox();
        if (m_ptrPerso->getType() == "Ennemie")//Tres mal r�alis� - a refaire plus tard
        {
            Unit* p{ m_ptrPerso->getPtrGroup()->perso };
            if (m_hitbox.intersects(p->getHitbox()) && !std::any_of(m_lstHit.begin(), m_lstHit.end(), [p](Unit* unit) {return unit == p; }))
            {
                p->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), m_ptrPerso->getADroite() ? m_knockback : -m_knockback);
                reset();
            }
        }
        else if (m_ptrPerso->getType() == "PJ")
        {
            for (Unit* ennemie : *m_ptrPerso->getPtrGroup()->ptrEnn)
            {
                if (m_hitbox.intersects(ennemie->getHitbox()))
                {
                    ennemie->hit(static_cast<int>(m_ptrPerso->getStat().AD * m_multiplier), m_ptrPerso->getADroite() ? m_knockback : -m_knockback);
                    reset();
                    break;
                }
            }
        }
    }
    else if (m_delay > 0)
    {
        reset();
        m_ptrPerso->setNumAtt((static_cast<uint16_t>(m_ptrPerso->getStat().AS.size() - 1) > m_ptrPerso->getNumAtt()) ? m_ptrPerso->getNumAtt() + 1 : 0);
    }
    m_delay -= 0.016;
}

void Attaque::reset()
{
    m_ptrPerso->getPtrGroup()->ptrAtt->erase(this);
    m_delay = 0;
    m_lstHit.clear();
}

void Attaque::update()
{
    switch (m_type)
    {
    case AllAttTypes::cac:
        Cac();
        break;
    case AllAttTypes::dist:
        Dist();
        break;
    default:
        break;
    }
    setTextureRect(sf::IntRect(0, int(!m_ptrPerso->getADroite() * m_img.getSize().y / 2), m_img.getSize().x, int(m_img.getSize().y / 2)));
}

/*
void Attaque::next()
{
    if (m_numAtt < m_ptrPerso->getStat().nbAtt)
    {
        m_numAtt += 1;
    }
    else
    {
        m_numAtt = 1;
    }
    m_lstHit.clear();
}
*/

double Attaque::getDelay() { return m_delay; }

double Attaque::getDelayStatic() { return m_delayStatic; }

double Attaque::getAvTmp() { return m_avTmp; }

void Attaque::setDelay(double val) { m_delay = val; }