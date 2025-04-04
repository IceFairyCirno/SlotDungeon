document.addEventListener('DOMContentLoaded', () => {
    const sections = document.querySelectorAll('section');
    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('visible');
            }
        });
    }, {
        threshold: 0.1
    });

    sections.forEach(section => {
        observer.observe(section);
    });

    const skillTags = document.querySelectorAll('.skill-tag');
    skillTags.forEach(tag => {
        tag.addEventListener('click', () => {
            tag.style.transform = 'scale(1.1)';
            setTimeout(() => {
                tag.style.transform = 'translateY(-2px)';
            }, 200);
        });
    });

    const subtitle = document.querySelector('.subtitle');
    const text = subtitle.textContent;
    subtitle.textContent = '';
    let i = 0;

    function typeWriter() {
        if (i < text.length) {
            subtitle.textContent += text.charAt(i);
            i++;
            setTimeout(typeWriter, 100);
        }
    }

    typeWriter();

    const prefersDarkScheme = window.matchMedia('(prefers-color-scheme: dark)');
    const currentTheme = localStorage.getItem('theme');

    if (currentTheme === 'dark') {
        document.body.classList.add('dark-theme');
    } else if (currentTheme === 'light') {
        document.body.classList.remove('dark-theme');
    }

    prefersDarkScheme.addListener((e) => {
        if (e.matches) {
            document.body.classList.add('dark-theme');
            localStorage.setItem('theme', 'dark');
        } else {
            document.body.classList.remove('dark-theme');
            localStorage.setItem('theme', 'light');
        }
    });

    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();
            document.querySelector(this.getAttribute('href')).scrollIntoView({
                behavior: 'smooth'
            });
        });
    });

    const themeToggle = document.getElementById('theme-toggle');
    const themeIcon = themeToggle.querySelector('.theme-icon');

    // 检查本地存储中的主题设置
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme === 'light') {
        document.body.setAttribute('data-theme', 'light');
        document.body.classList.add('light-theme');
        document.body.classList.remove('dark-theme');
        themeIcon.textContent = '☀️';
    } else {
        document.body.setAttribute('data-theme', 'dark');
        document.body.classList.add('dark-theme');
        document.body.classList.remove('light-theme');
        themeIcon.textContent = '🌙';
    }

    // 主题切换功能
    themeToggle.addEventListener('click', () => {
        const currentTheme = document.body.getAttribute('data-theme');
        const newTheme = currentTheme === 'light' ? 'dark' : 'light';
        
        document.body.setAttribute('data-theme', newTheme);
        document.body.classList.toggle('light-theme');
        document.body.classList.toggle('dark-theme');
        
        // 更新图标
        if (newTheme === 'light') {
            themeIcon.textContent = '☀️';
        } else {
            themeIcon.textContent = '🌙';
        }
        
        localStorage.setItem('theme', newTheme);
    });

    // Contact Form Handling
    const contactForm = document.getElementById('contact-form');
    if (contactForm) {
        contactForm.addEventListener('submit', function(e) {
            e.preventDefault();
            
            const formData = {
                email: document.getElementById('email').value,
                subject: document.getElementById('subject').value,
                message: document.getElementById('message').value
            };

            // 使用 mailto 链接发送邮件
            const mailtoLink = `mailto:dicky.ling1026@gmail.com?subject=${encodeURIComponent(formData.subject)}&body=${encodeURIComponent(`Email: ${formData.email}\n\nMessage:\n${formData.message}`)}`;
            window.location.href = mailtoLink;
        });
    }

    // 兴趣切换功能
    const interestBlocks = document.querySelectorAll('.interest-block');
    const prevBtn = document.querySelector('.interest-nav.prev');
    const nextBtn = document.querySelector('.interest-nav.next');
    const dotsContainer = document.querySelector('.interest-dots');
    let currentIndex = 0;

    // 创建导航点
    interestBlocks.forEach((_, index) => {
        const dot = document.createElement('div');
        dot.classList.add('interest-dot');
        if (index === 0) dot.classList.add('active');
        dot.addEventListener('click', () => goToSlide(index));
        dotsContainer.appendChild(dot);
    });

    const dots = document.querySelectorAll('.interest-dot');

    function updateSlides() {
        interestBlocks.forEach((block, index) => {
            block.classList.remove('active', 'prev', 'next');
            if (index === currentIndex) {
                block.classList.add('active');
            } else if (index === currentIndex - 1 || (currentIndex === 0 && index === interestBlocks.length - 1)) {
                block.classList.add('prev');
            } else if (index === currentIndex + 1 || (currentIndex === interestBlocks.length - 1 && index === 0)) {
                block.classList.add('next');
            }
        });

        dots.forEach((dot, index) => {
            dot.classList.toggle('active', index === currentIndex);
        });
    }

    function goToSlide(index) {
        currentIndex = index;
        updateSlides();
    }

    function nextSlide() {
        currentIndex = (currentIndex + 1) % interestBlocks.length;
        updateSlides();
    }

    function prevSlide() {
        currentIndex = (currentIndex - 1 + interestBlocks.length) % interestBlocks.length;
        updateSlides();
    }

    prevBtn.addEventListener('click', prevSlide);
    nextBtn.addEventListener('click', nextSlide);

    // 自动播放
    let autoplayInterval = setInterval(nextSlide, 5000);

    // 鼠标悬停时暂停自动播放
    const interestContainer = document.querySelector('.interest-container');
    interestContainer.addEventListener('mouseenter', () => {
        clearInterval(autoplayInterval);
    });

    interestContainer.addEventListener('mouseleave', () => {
        autoplayInterval = setInterval(nextSlide, 5000);
    });

    // 初始化显示
    updateSlides();
}); 