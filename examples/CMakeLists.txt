project ("examples")

add_executable(mpsc_examples
        "main.cpp"
        )

target_link_libraries(mpsc_examples
        PRIVATE
        mpsc)
